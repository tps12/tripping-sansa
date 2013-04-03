#ifndef __INCLUDE_ROUTING_TYPES_FIND_RESOURCE_FN_H__
#define __INCLUDE_ROUTING_TYPES_FIND_RESOURCE_FN_H__

struct found_resource;

typedef struct found_resource* (*find_resource_fn)(char const* path, char const** args);

#endif
