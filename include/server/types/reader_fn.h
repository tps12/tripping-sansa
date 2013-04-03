#ifndef __INCLUDE_SERVER_TYPES_READER_FN_H__
#define __INCLUDE_SERVER_TYPES_READER_FN_H__

typedef void* (*reader_fn)(char const* entity, size_t entity_length);

#endif
