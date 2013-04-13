#include <stdlib.h>

#include "server/types/result.h"

struct cookie;

static struct result* init_result(char* error, void* data, void (*free_data)(void* data), char* location, struct cookie* cookies)
{
    struct result* result = 0;

    result = malloc(sizeof(struct result));
    if (result) {
        result->error = error;
        result->data = data;
        result->free_data = free_data;
        result->location = location;
        result->cookies = cookies;
    }
    return result;
}

struct result* error_result(char* error)
{
    return init_result(error, 0, 0, 0, 0);
}

struct result* success_result(void* data, void (*free_data)(void* data), char* location, struct cookie* cookies)
{
    return init_result(0, data, free_data, location, cookies);
}

void free_result(struct result* result)
{
    if (result) {
        if (result->free_data)
            result->free_data(result->data);

        free(result->error);
        free(result->location);
    }
    free(result);
}
