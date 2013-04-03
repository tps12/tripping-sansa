#ifndef __INCLUDE_ROUTING_FUNCTIONS_ROUTE_RESOURCE_H__
#define __INCLUDE_ROUTING_FUNCTIONS_ROUTE_RESOURCE_H__

#include "routing/types/find_resource_fn.h"

struct path_route;
struct route_finder;

struct route_finder* route_resource(find_resource_fn find_resource, struct route_finder* next);

#endif
