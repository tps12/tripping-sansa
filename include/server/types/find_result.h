#ifndef __INCLUDE_ROUTING_TYPES_FIND_RESULT_H__
#define __INCLUDE_ROUTING_TYPES_FIND_RESULT_H__

struct find_result {
    int found;
    void* data;
    void (*free_data)(void* data);
};

#endif
