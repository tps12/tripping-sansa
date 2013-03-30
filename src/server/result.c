#include "server/types/result.h"

static struct result* init_result(char* error, void* data, void (*free_data)(void* data), char* location)
{
    struct result* result = 0;

    if (data && !free_data)
        return 0;

    result = malloc(sizeof(struct result));
    if (result) {
        result->error = error;
        result->data = data;
        result->free_data = free_data;
        result->location = location;
    }
    return result;
}

struct result* error_result(char* error)
{
    return init_result(error, 0, 0, 0);
}

struct result* success_result(void* data, void (*free_data)(void* data), char* location)
{
    return init_result(0, data, free_data, location);
}

void free_result(struct result* result)
{
    if (result) {
        if (result->data)
            result->free_data(result->data);

        free(result->error);
        free(result->location);
    }
    free(result);
}
