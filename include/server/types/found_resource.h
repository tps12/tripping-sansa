#ifndef __INCLUDE_SERVER_TYPES_FOUND_RESOURCE_H__
#define __INCLUDE_SERVER_TYPES_FOUND_RESOURCE_H__

struct resource;

struct found_resource {
    struct resource* resource;
    void* data;
};

#endif
