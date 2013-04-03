#ifndef __INCLUDE_ROUTING_FUNCTIONS_ROUTE_PATTERN_H__
#define __INCLUDE_ROUTING_FUNCTIONS_ROUTE_PATTERN_H__

struct path_route;
struct route_finder;

struct path_route* route_pattern(char const* pattern, struct route_finder* finders);

#endif
