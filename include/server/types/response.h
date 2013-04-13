#ifndef __INCLUDE_SERVER_TYPES_RESPONSE_H__
#define __INCLUDE_SERVER_TYPES_RESPONSE_H__

struct cookie;

struct response {
    unsigned short status;
    char* allow;
    char* location;
    struct cookie* cookies;
    char* entity;
    char* entity_type;
    size_t entity_length;
};

#endif
