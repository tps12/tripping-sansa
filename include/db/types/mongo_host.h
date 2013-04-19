#ifndef __INCLUDE_DB_TYPES_MONGO_HOST_H__
#define __INCLUDE_DB_TYPES_MONGO_HOST_H__

struct mongo_host {
    char* host;
    unsigned int port;
    char* username;
    char* password;
    char* database;
    struct mongo_host* next;
};

#endif
