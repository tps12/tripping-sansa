#include "routing/types/route.h"
#include "server/types/resource.h"
#include "server/types/result.h"
#include "server/types/entity.h"
#include "server/types/response.h"

#include "server/functions/free_result.h"

static struct response* build_response(int status, char* allow, char* location, char const* type, struct entity* entity)
{
    struct response* response = malloc(sizeof(struct response));
    if (response) {
        response->status = status;
        response->allow = allow;
        response->location = location;
        response->entity_type = (char*)type;
        response->entity = entity ? entity->data : 0;
        response->entity_length = entity ? entity->length : 0;
    }
    free(entity);
    return response;
}

static struct entity* build_entity(char* body)
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
        return build_response(406, 0, 0, "text/plain", build_entity(body));
    }

    return 0;
}

static struct response* internal_error()
{
    char* body = malloc(32);

    if (body)
        strncpy(body, "Check server logs", 31);

    return build_response(500, 0, 0, "text/plain", build_entity(body));
}

static struct response* success(char const* entity_type, struct entity* entity, char* location)
{
    int status = location ? 201 : entity ? 200 : 204;
    char* location_copy = 0;

    if (status == 201) {
        if (!(location_copy = malloc(2048)))
            return internal_error();

        strncpy(location_copy, location, 2048);
    }

    return build_response(status, 0, location_copy, entity_type, entity);
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
        return build_response(415, 0, 0, "text/plain", build_entity(body));
    }

    return 0;
}

static struct response* bad_request(char* error)
{
    char* body = malloc(2048);

    if (body) {
        strncpy(body, error, 2048);
        return build_response(400, 0, 0, "text/plain", build_entity(body));
    }

    return 0;
}

static struct response* respond_and_write(char const* path, void* data, respond_fn respond, struct writer* writers, char const* accept_type)
{
    struct result* result = 0;
    struct response* response = 0;
    struct writer* writer;

    if (!writers) {
        if (result = respond(path, data)) {
            response = result->error ? bad_request(result->error) :
                result->data ? internal_error() :
                success(0, 0, result->location);
            free_result(result);
            return response;
        }
        return 0;
    }

    for (writer = writers; writer; writer = writer->next)
        if (!strncmp(writer->type, accept_type, 256)) {
            if (result = respond(path, data)) {
                response = result->error ? bad_request(result->error) :
                    result->data ? success(writer->type, writer->writer(result->data), result->location) :
                    internal_error();
                free_result(result);
                return response;
            }
            return 0;
        }

    return not_acceptable(writers);
}

static struct response* read_and_respond(struct reader* readers, respond_fn respond, char const* path, char const* entity_type, char const* entity, size_t entity_length, struct writer* writers, char const* accept)
{
    struct reader* reader;
    if (!readers)
        return respond_and_write(path, 0, respond, writers, accept);
    for (reader = readers; reader; reader = reader->next)
        if (!strncmp(reader->type, entity_type, 256))
            return respond_and_write(path, reader->reader(entity, entity_length), respond, writers, accept);
    return unsupported_type(readers);
}

static struct response* respond_method(struct method* resource_method, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length, char const* accept)
{
    return resource_method ?
        (!strncmp(resource_method->method, method, 16) ?
            read_and_respond(resource_method->readers, resource_method->respond, path, entity_type, entity, entity_length, resource_method->writers, accept) :
            respond_method(resource_method->next, method, path, entity_type, entity, entity_length, accept)) :
        0;
}

static struct resource* resource_define(struct method* methods)
{
    struct resource* result = malloc(sizeof(struct resource));
    if (result)
        result->methods = methods;
    return result;
}

static struct reader* entity_reader(char const* type, reader_fn reader, struct reader* next)
{
    struct reader* result = malloc(sizeof(struct reader));
    if (result) {
        result->type = type;
        result->reader = reader;
        result->next = next;
    }
    return result;
}

static struct writer* entity_writer(char const* type, writer_fn writer, struct writer* next)
{
    struct writer* result = malloc(sizeof(struct writer));
    if (result) {
        result->type = type;
        result->writer = writer;
        result->next = next;
    }
    return result;
}

static struct method* resource_method(char const* method, struct reader* readers, struct writer* writers, respond_fn respond, struct method* next)
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

static void free_resource(struct resource* resource)
{
    free_methods(resource->methods);
    free(resource);
}

static void free_routes(struct route* route)
{
    if (route) {
        free(route->path_route);
        free_routes(route->next);
    }
    free(route);
}

static void free_response(struct response* response)
{
    if (response) {
        if (response->allow)
            free(response->allow);
        if (response->location)
            free(response->location);
        if (response->entity_length)
            free(response->entity);
    }
    free(response);
}

static struct route* path_route(struct path_route* route, struct route* next)
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
        return build_response(405, allow, 0, "text/plain", build_entity(body));
    }

    free(allow);
    return 0;
}

static struct response* not_found()
{
    return build_response(404, 0, 0, 0, 0);
}

struct response* handle_request(struct route* routes, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length, char const* accept)
{
    struct route* route;
    struct resource* resource = 0;
    struct response* response;

    for (route = routes; route; route = route->next) {
        resource = route_path((struct path_route const*)route->path_route, path);
        if (resource) {
            if (response = respond_method(resource->methods, method, path, entity_type, entity, entity_length, accept))
                return response;
            else
                return not_allowed(resource->methods);
        }
    }

    return not_found();
}
