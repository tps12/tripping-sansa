#include <stdlib.h>
#include <string.h>

#include "logging.h"

#include "routing/types/route.h"

#include "routing/functions/free_route.h"
#include "routing/functions/route_path.h"

#include "server/types/found_resource.h"
#include "server/types/cookie.h"
#include "server/types/resource.h"
#include "server/types/result.h"
#include "server/types/entity.h"
#include "server/types/response.h"

#include "server/functions/free_result.h"

static struct response* build_response(int status, char* allow, char* location, char const* type, struct entity* entity, struct cookie* cookies)
{
    struct response* response = malloc(sizeof(struct response));
    if (response) {
        response->status = status;
        response->allow = allow;
        response->location = location;
        response->cookies = cookies;
        response->entity_type = (char*)type;
        response->entity = entity ? entity->data : 0;
        response->entity_length = entity ? entity->length : 0;
    }
    free(entity);
    return response;
}

struct entity* build_entity(char* body)
{
    struct entity* entity = malloc(sizeof(struct entity));

    if (entity) {
        entity->data = body;
        entity->length = body ? strlen(body) : 0;
    }

    return entity;
}

static struct response* not_acceptable(struct writer* writers)
{
    int first = 1;
    char* body = malloc(2048);

    if (body) {
        strncpy(body, "Response types supported: ", 2048);
        for ( ; writers; writers = writers->next) {
            if (!first)
                strncat(body, ", ", 2);
            strncat(body, writers->type, 256);
            first = 0;
        }
        return build_response(406, 0, 0, "text/plain", build_entity(body), 0);
    }

    return 0;
}

static struct response* internal_error()
{
    char* body = malloc(32);

    if (body)
        strncpy(body, "Check server logs", 31);

    return build_response(500, 0, 0, "text/plain", build_entity(body), 0);
}

static struct response* success(char const* entity_type, struct entity* entity, char* location, struct cookie* cookies)
{
    int status = location ? 201 : entity ? 200 : 204;
    char* location_copy = 0;

    if (status == 201) {
        if (!(location_copy = malloc(2048)))
            return internal_error();

        strncpy(location_copy, location, 2048);
    }

    return build_response(status, 0, location_copy, entity_type, entity, cookies);
}

static struct response* unsupported_type(struct reader* readers)
{
    int first = 1;
    char* body = malloc(2048);

    if (body) {
        strncpy(body, "Media types supported: ", 2048);
        for ( ; readers; readers = readers->next) {
            if (!first)
                strncat(body, ", ", 2);
            strncat(body, readers->type, 256);
            first = 0;
        }
        return build_response(415, 0, 0, "text/plain", build_entity(body), 0);
    }

    return 0;
}

static struct response* bad_request(char* error)
{
    char* body = malloc(2048);

    if (body) {
        strncpy(body, error, 2048);
        return build_response(400, 0, 0, "text/plain", build_entity(body), 0);
    }

    return 0;
}

static int count_commas(char const* string)
{
    int i, n = 0;
    if (string)
        for (i = 0; string[i]; i++)
            if (string[i] == ',')
                n++;
    return n;
}

static void free_strings(char** strings)
{
    int i;
    for (i = 0; strings[i]; i++)
        free(strings[i]);
    free(strings);
}

static char** split_accept(char const* accept_string)
{
    int i, j, len, n = count_commas(accept_string);
    char** types = calloc(n + 2, sizeof(char *));
    char* current;

    if (types) {
        current = (char*)accept_string;
        for (i = 0; i < n + 1; i++) {
            while (current && isspace(*current))
                current++;
            len = 0;
            if (current)
                while (current[len] && current[len] != ',' && current[len] != ';')
                    len++;
            for (j = len; current && isspace(current[j]); j--)
                ;
            types[i] = malloc(j+1);
            if (!types[i]) {
                free_strings(types);
                types = 0;
                break;
            }
            if (current)
                strncpy(types[i], current, j);
            types[i][j] = 0;
            if (current) {
                while (current[len] && current[len] != ',')
                    len++;
                current += len + 1;
            }
        }
    }

    if (!types)
        log_error("Couldn't allocate space for accept types");
    return types;
}

static struct response* respond_and_write(char const* path, void* resource_data, void* data, respond_fn respond, struct writer* writers, char const* accept_type)
{
    struct result* result = 0;
    struct response* response = 0;
    char** accept_types = 0;
    int i;
    struct writer* writer;

    if (!writers) {
        if (result = respond(path, resource_data, data)) {
            response = result->error ? bad_request(result->error) :
                result->data ? internal_error() :
                success(0, 0, result->location, result->cookies);
            free_result(result);
            return response;
        }
        return 0;
    }

    for (writer = writers; writer; writer = writer->next) {
        accept_types = split_accept(accept_type);
        for (i = 0; accept_types[i]; i++) {
            if (!strncmp(writer->type, accept_types[i], 256)) {
                if (result = respond(path, resource_data, data)) {
                    response = result->error ? bad_request(result->error) :
                        result->data ? success(writer->type, writer->writer(result->data), result->location, result->cookies) :
                        internal_error();
                    free_result(result);
                    free_strings(accept_types);
                    return response;
                }
                free_strings(accept_types);
                return 0;
            }
        }
        free_strings(accept_types);
    }

    log_info("No matching type in Accept '%s'\n", accept_type);
    return not_acceptable(writers);
}

static struct response* read_and_respond(struct reader* readers, respond_fn respond, char const* path, void* data, char const* entity_type, char const* entity, size_t entity_length, struct writer* writers, char const* accept)
{
    struct reader* reader;
    if (!readers)
        return respond_and_write(path, data, 0, respond, writers, accept);
    for (reader = readers; reader; reader = reader->next)
        if (!strncmp(reader->type, entity_type, 256))
            return respond_and_write(path, data, reader->reader(entity, entity_length), respond, writers, accept);
    return unsupported_type(readers);
}

static struct response* respond_method(struct method* resource_method, char const* method, char const* path, void* data, char const* entity_type, char const* entity, size_t entity_length, char const* accept)
{
    return resource_method ?
        (!strncmp(resource_method->method, method, 16) ?
            read_and_respond(resource_method->readers, resource_method->respond, path, data, entity_type, entity, entity_length, resource_method->writers, accept) :
            respond_method(resource_method->next, method, path, data, entity_type, entity, entity_length, accept)) :
        0;
}

struct resource* resource_define(find_resource_fn find, struct method* methods)
{
    struct resource* result = malloc(sizeof(struct resource));
    if (result) {
        result->find = find;
        result->methods = methods;
    }
    return result;
}

struct reader* entity_reader(char const* type, reader_fn reader, struct reader* next)
{
    struct reader* result = malloc(sizeof(struct reader));
    if (result) {
        result->type = type;
        result->reader = reader;
        result->next = next;
    }
    return result;
}

struct writer* entity_writer(char const* type, writer_fn writer, struct writer* next)
{
    struct writer* result = malloc(sizeof(struct writer));
    if (result) {
        result->type = type;
        result->writer = writer;
        result->next = next;
    }
    return result;
}

struct method* resource_method(char const* method, struct reader* readers, struct writer* writers, respond_fn respond, struct method* next)
{
    struct method* result = malloc(sizeof(struct method));
    if (result) {
        result->method = method;
        result->readers = readers;
        result->writers = writers;
        result->respond = respond;
        result->next = next;
    }
    return result;
}

static void free_readers(struct reader* reader)
{
    if (reader)
        free_readers(reader->next);
    free(reader);
}

static void free_writers(struct writer* writer)
{
    if (writer)
        free_writers(writer->next);
    free(writer);
}

static void free_methods(struct method* method)
{
    if (method) {
        free_readers(method->readers);
        free_writers(method->writers);
        free_methods(method->next);
    }
    free(method);
}

void free_resource(struct resource* resource)
{
    free_methods(resource->methods);
    free(resource);
}

void free_routes(struct route* route)
{
    if (route) {
        free_route(route->path_route);
        free_routes(route->next);
    }
    free(route);
}

void free_cookies(struct cookie* cookie)
{
    if (cookie) {
        free(cookie->name);
        free(cookie->value);
        free_cookies(cookie->next);
    }
    free(cookie);
}

void free_response(struct response* response)
{
    if (response) {
        if (response->allow)
            free(response->allow);
        if (response->location)
            free(response->location);
        if (response->cookies)
            free_cookies(response->cookies);
        if (response->entity_length)
            free(response->entity);
    }
    free(response);
}

struct route* path_route(struct path_route* route, struct route* next)
{
    struct route* result = malloc(sizeof(struct route));
    if (result) {
        result->path_route = route;
        result->next = next;
    }
    return result;
}

static struct response* not_allowed(struct method* methods)
{
    int first = 1;
    char* allow = malloc(128);
    char* body = malloc(128);

    if (allow && body) {
        strncpy(body, "Method not allowed", 128);
        strncpy(allow, "", 1);
        for ( ; methods; methods = methods->next) {
            if (!first)
                strncat(allow, ", ", 2);
            strncat(allow, methods->method, 128 - (strlen(allow)));
            first = 0;
        }
        return build_response(405, allow, 0, "text/plain", build_entity(body), 0);
    }

    free(allow);
    return 0;
}

static struct response* not_found()
{
    return build_response(404, 0, 0, 0, 0, 0);
}

struct response* handle_request(struct route* routes, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length, char const* accept)
{
    struct route* route;
    struct found_resource* resource = 0;
    struct response* response = 0;

    for (route = routes; route; route = route->next) {
        resource = route_path((struct path_route const*)route->path_route, path);
        if (resource) {
            response = respond_method(resource->resource->methods, method, path, resource->data, entity_type, entity, entity_length, accept);
            if (!response)
                response = not_allowed(resource->resource->methods);
            break;
        }
    }
    if (resource && resource->free_data)
        resource->free_data(resource->data);
    free(resource);

    if (!response)
        response = not_found();

    return response;
}
