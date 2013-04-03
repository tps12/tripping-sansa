#ifndef __INCLUDE_SERVER_FUNCTIONS_HANDLE_REQUEST_H__
#define __INCLUDE_SERVER_FUNCTIONS_HANDLE_REQUEST_H__

struct response;
struct route;

struct response* handle_request(struct route* routes, char const* method, char const* path, char const* entity_type, char const* entity, size_t entity_length, char const* accept);

#endif
