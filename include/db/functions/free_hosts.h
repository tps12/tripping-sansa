#ifndef __INCLUDE_DB_FUNCTIONS_FREE_HOSTS_H__
#define __INCLUDE_DB_FUNCTIONS_FREE_HOSTS_H__

struct mongo_host;

void free_hosts(struct mongo_host* hosts);

#endif
