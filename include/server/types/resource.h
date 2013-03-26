#ifndef __INCLUDE_SERVER_TYPES_RESOURCE_H__
#define __INCLUDE_SERVER_TYPES_RESOURCE_H__

#include "response.h"

typedef response_t* (*respond_fn)(char const* path, void* data);

typedef void* (*reader_fn)(char const* entity, size_t entity_length);

typedef response_t* (*writer_fn)(void* data);

typedef struct type_reader_t {
    char const* type;
    reader_fn reader;
    struct type_reader_t* next;
} reader_t;

typedef struct type_writer_t {
    char const* type;
    writer_fn writer;
    struct type_writer_t* next;
} writer_t;

typedef struct method_t {
    char const* method;
    reader_t* readers;
    writer_t* writers;
    respond_fn respond;
    struct method_t* next;
} method_t;

typedef struct {
    method_t* methods;
} resource_t;

#endif
