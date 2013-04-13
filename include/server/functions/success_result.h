#ifndef __INCLUDE_SERVER_FUNCTIONS_SUCCESS_RESULT_H__
#define __INCLUDE_SERVER_FUNCTIONS_SUCCESS_RESULT_H__

struct cookie;
struct result;

struct result* success_result(char* entity, void (*free_data)(void* data), char* location, struct cookie* cookies);

#endif
