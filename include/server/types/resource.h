#ifndef __INCLUDE_SERVER_TYPES_RESOURCE_H__
#define __INCLUDE_SERVER_TYPES_RESOURCE_H__

#include "routing/types/find_resource_fn.h"
#include "server/types/reader_fn.h"
#include "server/types/respond_fn.h"
#include "server/types/writer_fn.h"

struct entity;
struct response;
struct result;

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
    find_resource_fn find;
    struct method* methods;
};

#endif
