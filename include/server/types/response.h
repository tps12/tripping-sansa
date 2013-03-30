#ifndef __INCLUDE_SERVER_TYPES_RESPONSE_H__
#define __INCLUDE_SERVER_TYPES_RESPONSE_H__

struct response {
    unsigned short status;
    char* allow;
    char* location;
    char* entity;
    char* entity_type;
    size_t entity_length;
};

#endif
