#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "adapters/libmicrohttpd.h"

#include "resource/functions/found_resource.h"
#include "resource/functions/resource_not_found.h"

#include "routing/functions/route_pattern.h"
#include "routing/functions/route_resource.h"

#include "server/functions/build_entity.h"
#include "server/functions/entity_reader.h"
#include "server/functions/entity_writer.h"
#include "server/functions/free_resource.h"
#include "server/functions/free_response.h"
#include "server/functions/free_routes.h"
#include "server/functions/path_route.h"
#include "server/functions/resource_define.h"
#include "server/functions/resource_method.h"
#include "server/functions/success_result.h"

#define PAGE "<html><head><title>hello</title></head><body>there</body></html>"
#define EVEN "<html><head><title>hello</title></head><body>%d</body></html>"

static struct find_result* find_hello(char const* path, char const** args)
{
    return found_resource(0, 0);
}

static struct result* get_hello(char const* path, void* resource_data, void* data)
{
    char* body = malloc(256);

    if (body) {
      sprintf(body, PAGE);
      return success_result(body, &free, 0);
    }

    return 0;
}

static struct result* put_hello(char const* path, void* resource_data, void* data)
{
    return success_result(0, 0, 0);
}

static struct result* post_hello(char const* path, void* resource_data, void* data)
{
    char* location = malloc(20);

    if (location) {
      sprintf(location, "/hello");
      return success_result(0, 0, location);
    }

    return 0;
}

static void* read_hello(char const* entity, size_t length)
{
    return 0;
}

static struct entity* write_hello(void* data)
{
    char* body = malloc(strlen((char*)data) + 1);

    if (body) {
      strncpy(body, data, strlen((char*)data));
      return build_entity(body);
    }

    return 0;
}

static struct resource* even;

static struct find_result* find_even(char const* path, char const** args)
{
    int* data = 0;
    int value = atoi(args[0]);

    if (!(value % 2)) {
        data = malloc(sizeof(int));

        if (data) {
            *data = value;
            return found_resource(data, &free);
        }
        else
            return 0;
    }
    else
        return resource_not_found();
}

static struct result* get_even(char const* path, void* value, void* data)
{
    char* body;

    if (value) {
        body = malloc(256);

        if (body) {
            sprintf(body, EVEN, *((int*)value));
            return success_result(body, &free, 0);
        }
    }

    return 0;
}

static struct entity* write_even(void* data)
{
    char* body = malloc(strlen((char*)data) + 1);

    if (body) {
        strncpy(body, data, strlen((char*)data) + 1);
        return build_entity(body);
    }

    return 0;
}

int main(int argc, char const** argv)
{
    struct route* routes;
    int port;

    if (argc != 2)
    {
        printf ("%s PORT\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);

    routes =
        path_route(route_pattern("^/hello$",
            route_resource(resource_define(&find_hello,
                resource_method("GET", 0, entity_writer("text/html", &write_hello, 0), &get_hello,
                resource_method("PUT", entity_reader("text/plain", &read_hello, 0), 0, &put_hello,
                resource_method("POST", 0, 0, &post_hello,
            0)))),
        0)),
        path_route(route_pattern("^/evens/([0-9]+)$",
            route_resource(resource_define(&find_even,
                resource_method("GET", 0, entity_writer("text/html", &write_even, 0), &get_even,
            0)),
        0)),
    0));

    run_app(port, routes);

    free_routes(routes);

    return 0;
}
