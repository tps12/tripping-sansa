#ifndef __INCLUDE_ROUTING_FUNCTIONS_ROUTE_PATH_H__
#define __INCLUDE_ROUTING_FUNCTIONS_ROUTE_PATH_H__

struct path_route;
struct found_resource;

struct found_resource* route_path(struct path_route const* route, char const* path);

#endif
