#ifndef __INCLUDE_SERVER_TYPES_RESULT_H__
#define __INCLUDE_SERVER_TYPES_RESULT_H__

struct result {
    char* error;
    char* location;
    void* data;
    void (*free_data)(void* data);
};

#endif
