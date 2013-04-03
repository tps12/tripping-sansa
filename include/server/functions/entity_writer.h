#ifndef __INCLUDE_SERVER_FUNCTIONS_ENTITY_WRITER_H__
#define __INCLUDE_SERVER_FUNCTIONS_ENTITY_WRITER_H__

#include "server/types/writer_fn.h"

struct writer;

struct writer* entity_writer(char const* type, writer_fn writer, struct writer* next);

#endif
