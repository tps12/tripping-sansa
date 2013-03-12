#ifndef __INCLUDE_SERVER_TYPES_ROUTE_H__
#define __INCLUDE_SERVER_TYPES_ROUTE_H__

struct path_route_t;

typedef struct route_t {
    struct path_route_t* path_route;
    struct route_t* next;
} route_t;

#endif
