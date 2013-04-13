#ifndef __INCLUDE_SERVER_TYPES_RESULT_H__
#define __INCLUDE_SERVER_TYPES_RESULT_H__

struct cookie;

struct result {
    char* error;
    char* location;
    struct cookie* cookies;
    void* data;
    void (*free_data)(void* data);
};

#endif
