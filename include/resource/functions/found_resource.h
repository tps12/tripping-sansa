#ifndef __INCLUDE_RESOURCE_FUNCTIONS_FOUND_RESOURCE_H__
#define __INCLUDE_RESOURCE_FUNCTIONS_FOUND_RESOURCE_H__

struct find_result;

struct find_result* found_resource(void* data, void (*free_data)(void *));

#endif
