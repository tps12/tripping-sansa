#include "routing/types/route.h"
#include "server/types/resource.h"
#include "server/types/response.h"

static struct response* not_acceptable(struct writer* writers)
{
    int first = 1;
    struct response* response = malloc(sizeof(struct response));
    response->status = 406;
    response->allow = 0;
    response->entity = malloc(2048);
    strncpy(response->entity, "Response types supported: ", 2048);
    for ( ; writers; writers = writers->next) {
        if (!first)
            strncat(response->entity, "; ", 2);
        strncat(response->entity, writers->type, 256);
        first = 0;
    }
    response->entity_length = strnlen(response->entity, 2048);
    return response;
}

static struct response* unsupported_type(struct reader* readers)
{
    int first = 1;
    struct response* response = malloc(sizeof(struct response));
    response->status = 415;
    response->allow = 0;
    response->entity = malloc(2048);
    strncpy(response->entity, "Media types supported: ", 2048);
    for ( ; readers; readers = readers->next) {
        if (!first)
            strncat(response->entity, "; ", 2);
        strncat(response->entity, readers->type, 256);
        first = 0;
    }
    response->entity_length = strnlen(response->entity, 2048);
    return response;
}

static struct response* respond_and_write(char const* path, void* data, respond_fn respond, struct writer* writers, char const* accept_type)
{
    struct writer* writer;
    if (!writers)
        return respond(path, data);
    for (writer = writers; writer; writer = writer->next)
        if (!strncmp(writer->type, accept_type, 256))
            return writer->writer(respond(path, data));
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
    struct response* response = malloc(sizeof(struct response));
    response->status = 405;
    response->allow = malloc(128);
    strncpy(response->allow, "Method not allowed", 128);
    for ( ; methods; methods = methods->next) {
        if (!first)
            strncat(response->allow, ", ", 2);
        strncat(response->allow, methods->method, 16);
        first = 0;
    }
    response->entity = malloc(128);
    strncpy(response->entity, "Method not allowed", 128);
    response->entity_length = strnlen(response->entity, 128);
    return response;
}

static struct response* not_found()
{
    struct response* response = malloc(sizeof(struct response));
    response->status = 404;
    response->allow = 0;
    response->entity = 0;
    response->entity_length = 0;
    return response;
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
