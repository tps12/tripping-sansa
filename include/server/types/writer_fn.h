#ifndef __INCLUDE_SERVER_TYPES_WRITER_FN_H__
#define __INCLUDE_SERVER_TYPES_WRITER_FN_H__

struct entity;

typedef struct entity* (*writer_fn)(void* data);

#endif
