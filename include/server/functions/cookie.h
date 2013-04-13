#ifndef __INCLUDE_SERVER_FUNCTIONS_COOKIE_H__
#define __INCLUDE_SERVER_FUNCTIONS_COOKIE_H__

struct cookie;

struct cookie* cookie(char* name, char* value, struct cookie* next);

#endif