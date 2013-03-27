#ifndef __INCLUDE_SERVER_TYPES_RESOURCE_H__
#define __INCLUDE_SERVER_TYPES_RESOURCE_H__

struct response;

typedef struct response* (*respond_fn)(char const* path, void* data);

typedef void* (*reader_fn)(char const* entity, size_t entity_length);

typedef struct response* (*writer_fn)(void* data);

struct reader {
    char const* type;
    reader_fn reader;
    struct reader* next;
};

struct writer {
    char const* type;
    writer_fn writer;
    struct writer* next;
};

struct method {
    char const* method;
    struct reader* readers;
    struct writer* writers;
    respond_fn respond;
    struct method* next;
};

struct resource {
    struct method* methods;
};

#endif
