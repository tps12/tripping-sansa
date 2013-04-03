#ifndef __INCLUDE_ADAPTERS_LIBMICROHTTPD_H__
#define __INCLUDE_ADAPTERS_LIBMICROHTTPD_H__

struct route;

void run_app(int port, struct route* routes);

#endif
