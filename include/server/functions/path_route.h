#ifndef __INCLUDE_SERVER_FUNCTIONS_PATH_ROUTE_H__
#define __INCLUDE_SERVER_FUNCTIONS_PATH_ROUTE_H__

struct path_route;
struct route;

struct route* path_route(struct path_route* route, struct route* next);

#endif
