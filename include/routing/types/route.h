#ifndef __INCLUDE_SERVER_TYPES_ROUTE_H__
#define __INCLUDE_SERVER_TYPES_ROUTE_H__

struct path_route;

struct route {
    struct path_route* path_route;
    struct route* next;
};

#endif
