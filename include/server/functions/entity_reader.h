#ifndef __INCLUDE_SERVER_FUNCTIONS_ENTITY_READER_H__
#define __INCLUDE_SERVER_FUNCTIONS_ENTITY_READER_H__

#include "server/types/reader_fn.h"

struct reader;

struct reader* entity_reader(char const* type, reader_fn reader, struct reader* next);

#endif
