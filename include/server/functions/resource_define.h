#ifndef __INCLUDE_SERVER_FUNCTIONS_RESOURCE_DEFINE_H__
#define __INCLUDE_SERVER_FUNCTIONS_RESOURCE_DEFINE_H__

#include "routing/types/find_resource_fn.h"

struct method;
struct resource;

struct resource* resource_define(find_resource_fn, struct method* methods);

#endif
