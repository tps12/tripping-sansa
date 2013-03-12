#ifndef __INCLUDE_SERVER_TYPES_RESOURCE_H__
#define __INCLUDE_SERVER_TYPES_RESOURCE_H__

#include "response.h"

typedef response_t* (*respond_fn)(char const* path, char const* type, char const* entity, size_t entity_length);

typedef struct method_t {
    char const* method;
    respond_fn respond;
    struct method_t* next;
} method_t;

typedef struct {
    method_t* methods;
} resource_t;

#endif
