#ifndef __INCLUDE_SERVER_FUNCTIONS_RESOURCE_METHOD_H__
#define __INCLUDE_SERVER_FUNCTIONS_RESOURCE_METHOD_H__

#include "server/types/respond_fn.h"

struct method;
struct reader;
struct writer;

struct method* resource_method(char const* method, struct reader* readers, struct writer* writers, respond_fn respond, struct method* next);

#endif
