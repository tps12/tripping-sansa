#ifndef __INCLUDE_SERVER_TYPES_RESPOND_FN_H__
#define __INCLUDE_SERVER_TYPES_RESPOND_FN_H__

struct result;

typedef struct result* (*respond_fn)(char const* path, void* data);

#endif
