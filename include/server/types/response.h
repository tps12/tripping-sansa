#ifndef __INCLUDE_SERVER_TYPES_RESPONSE_H__
#define __INCLUDE_SERVER_TYPES_RESPONSE_H__

typedef struct {
    unsigned short status;
    char* allow;
    char* entity;
    size_t entity_length;
} response_t;

#endif
