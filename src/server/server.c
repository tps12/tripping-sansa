#include "routing/types/route.h"
#include "server/types/resource.h"
#include "server/types/response.h"

static response_t* respond_method(method_t* resource_method, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length)
{
    return resource_method ?
        (!strncmp(resource_method->method, method, 16) ?
            resource_method->respond(path, entity_type, entity, entity_length) :
            respond_method(resource_method->next, method, path, entity_type, entity, entity_length)) :
        0;
}

static resource_t* resource_define(method_t* methods)
{
    resource_t* result = malloc(sizeof(resource_t));
    if (result)
        result->methods = methods;
    return result;
}

static method_t* resource_method(char const* method, respond_fn respond, method_t* next)
{
    method_t* result = malloc(sizeof(method_t));
    if (result) {
        result->method = method;
        result->respond = respond;
        result->next = next;
    }
    return result;
}

static void free_methods(method_t* method)
{
    if (method)
        free_methods(method->next);
    free(method);
}

static void free_resource(resource_t* resource)
{
    free_methods(resource->methods);
}

static void free_routes(route_t* route)
{
    if (route)
        free_routes(route->next);
    free(route);
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

response_t* handle_request(route_t* routes, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length)
{
    route_t* route;
    resource_t* resource = 0;

    for (route = routes; route; route = route->next) {
        resource = route_path((path_route_t const*)route->path_route, path);
        if (resource)
            return respond_method(resource->methods, method, path, entity_type, entity, entity_length);
    }

    return 0;
}
