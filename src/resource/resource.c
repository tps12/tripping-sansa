#include <stdlib.h>

#include "server/types/find_result.h"
#include "server/types/found_resource.h"
#include "server/types/resource.h"

struct find_result* resource_not_found()
{
    struct find_result* result = malloc(sizeof(struct found_resource));

    if (result) {
        result->found = 0;
        result->data = 0;
        result->free_data = 0;
    }

    return result;
}

struct find_result* found_resource(void* data, void (*free_data)(void* data))
{
    struct find_result* result = malloc(sizeof(struct found_resource));

    if (result) {
        result->found = 1;
        result->data = data;
        result->free_data = free_data;
    }

    return result;
}

void free_find_result(struct find_result* result)
{
    free(result);
}

struct found_resource* find_resource(struct resource* resource, char const* path, char const** args)
{
    struct find_result* result = resource->find(path, args);
    struct found_resource* found_resource = 0;

    if (result) {
        if (result->found && (found_resource = malloc(sizeof(struct found_resource)))) {
            found_resource->resource = resource;
            found_resource->data = result->data;
            found_resource->free_data = result->free_data;
        }
        free_find_result(result);
        return found_resource;
    }
    else
        return 0;
}

