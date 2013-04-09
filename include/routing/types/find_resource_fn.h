#ifndef __INCLUDE_ROUTING_TYPES_FIND_RESOURCE_FN_H__
#define __INCLUDE_ROUTING_TYPES_FIND_RESOURCE_FN_H__

struct find_result;

typedef struct find_result* (*find_resource_fn)(char const* path, char const** args);

#endif
