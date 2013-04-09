#ifndef __INCLUDE_RESOURCE_FUNCTIONS_FIND_RESOURCE_H__
#define __INCLUDE_RESOURCE_FUNCTIONS_FIND_RESOURCE_H__

struct found_resource;
struct resource;

struct found_resource* find_resource(struct resource* resource, char const* path, char const** args);

#endif
