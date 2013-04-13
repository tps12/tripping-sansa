#ifndef __INCLUDE_SERVER_TYPES_COOKIE_H__
#define __INCLUDE_SERVER_TYPES_COOKIE_H__

struct cookie {
	char* name;
	char* value;
	struct cookie* next;
};

#endif
