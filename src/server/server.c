#include "routing/types/route.h"
#include "server/types/resource.h"
#include "server/types/response.h"

static response_t* not_acceptable(writer_t* writers)
{
    int first = 1;
    response_t* response = malloc(sizeof(response_t));
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

static response_t* unsupported_type(reader_t* readers)
{
    int first = 1;
    response_t* response = malloc(sizeof(response_t));
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

static response_t* respond_and_write(char const* path, void* data, respond_fn respond, writer_t* writers, char const* accept_type)
{
    writer_t* writer;
    if (!writers)
        return respond(path, data);
    for (writer = writers; writer; writer = writer->next)
        if (!strncmp(writer->type, accept_type, 256))
            return writer->writer(respond(path, data));
    return not_acceptable(writers);
}

static response_t* read_and_respond(reader_t* readers, respond_fn respond, char const* path, char const* entity_type, char const* entity, size_t entity_length, writer_t* writers, char const* accept)
{
    reader_t* reader;
    if (!readers)
        return respond_and_write(path, 0, respond, writers, accept);
    for (reader = readers; reader; reader = reader->next)
        if (!strncmp(reader->type, entity_type, 256))
            return respond_and_write(path, reader->reader(entity, entity_length), respond, writers, accept);
    return unsupported_type(readers);
}

static response_t* respond_method(method_t* resource_method, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length, char const* accept)
{
    return resource_method ?
        (!strncmp(resource_method->method, method, 16) ?
            read_and_respond(resource_method->readers, resource_method->respond, path, entity_type, entity, entity_length, resource_method->writers, accept) :
            respond_method(resource_method->next, method, path, entity_type, entity, entity_length, accept)) :
        0;
}

static resource_t* resource_define(method_t* methods)
{
    resource_t* result = malloc(sizeof(resource_t));
    if (result)
        result->methods = methods;
    return result;
}

static reader_t* entity_reader(char const* type, reader_fn reader, reader_t* next)
{
    reader_t* result = malloc(sizeof(reader_t));
    if (result) {
        result->type = type;
        result->reader = reader;
        result->next = next;
    }
    return result;
}

static writer_t* entity_writer(char const* type, writer_fn writer, writer_t* next)
{
    writer_t* result = malloc(sizeof(writer_t));
    if (result) {
        result->type = type;
        result->writer = writer;
        result->next = next;
    }
    return result;
}

static method_t* resource_method(char const* method, reader_t* readers, writer_t* writers, respond_fn respond, method_t* next)
{
    method_t* result = malloc(sizeof(method_t));
    if (result) {
        result->method = method;
        result->readers = readers;
        result->writers = writers;
        result->respond = respond;
        result->next = next;
    }
    return result;
}

static void free_readers(reader_t* reader)
{
    if (reader)
        free_readers(reader->next);
    free(reader);
}

static void free_writers(writer_t* writer)
{
    if (writer)
        free_writers(writer->next);
    free(writer);
}

static void free_methods(method_t* method)
{
    if (method) {
        free_readers(method->readers);
        free_writers(method->writers);
        free_methods(method->next);
    }
    free(method);
}

static void free_resource(resource_t* resource)
{
    free_methods(resource->methods);
    free(resource);
}

static void free_routes(route_t* route)
{
    if (route) {
        free(route->path_route);
        free_routes(route->next);
    }
    free(route);
}

static void free_response(response_t* response)
{
    if (response) {
        if (response->allow)
            free(response->allow);
        if (response->entity_length)
            free(response->entity);
    }
    free(response);
}

static route_t* path_route(path_route_t* route, route_t* next)
{
    route_t* result = malloc(sizeof(route_t));
    if (result) {
        result->path_route = route;
        result->next = next;
    }
    return result;
}

static response_t* not_allowed(method_t* methods)
{
    int first = 1;
    response_t* response = malloc(sizeof(response_t));
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

static response_t* not_found()
{
    response_t* response = malloc(sizeof(response_t));
    response->status = 404;
    response->allow = 0;
    response->entity = 0;
    response->entity_length = 0;
    return response;
}

response_t* handle_request(route_t* routes, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length, char const* accept)
{
    route_t* route;
    resource_t* resource = 0;
    response_t* response;

    for (route = routes; route; route = route->next) {
        resource = route_path((path_route_t const*)route->path_route, path);
        if (resource) {
            if (response = respond_method(resource->methods, method, path, entity_type, entity, entity_length, accept))
                return response;
            else
                return not_allowed(resource->methods);
        }
    }

    return not_found();
}
