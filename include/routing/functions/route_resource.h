#ifndef __INCLUDE_ROUTING_FUNCTIONS_ROUTE_RESOURCE_H__
#define __INCLUDE_ROUTING_FUNCTIONS_ROUTE_RESOURCE_H__

struct resource;
struct route_finder;

struct route_finder* route_resource(struct resource* resource, struct route_finder* next);

#endif
