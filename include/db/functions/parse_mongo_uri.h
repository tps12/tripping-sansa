#ifndef __INCLUDE_DB_FUNCTIONS_PARSE_MONGO_URI_H__
#define __INCLUDE_DB_FUNCTIONS_PARSE_MONGO_URI_H__

struct mongo_host;

struct mongo_host* parse_mongo_uri(char const* uri);

#endif
