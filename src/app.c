#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "adapters/libmicrohttpd.h"

#include "resource/functions/found_resource.h"
#include "resource/functions/resource_not_found.h"

#include "routing/functions/route_pattern.h"
#include "routing/functions/route_resource.h"

#include "server/functions/build_entity.h"
#include "server/functions/cookie.h"
#include "server/functions/entity_reader.h"
#include "server/functions/entity_writer.h"
#include "server/functions/free_resource.h"
#include "server/functions/free_response.h"
#include "server/functions/free_routes.h"
#include "server/functions/path_route.h"
#include "server/functions/resource_define.h"
#include "server/functions/resource_method.h"
#include "server/functions/success_result.h"

#define EVEN "{ \"value\": %d }"

static char* app_content = 0;

static struct find_result* find_app(char const* path, char const** args)
{
    char* data = 0;

    if (args[0]) {
        data = malloc(strlen(args[0]) + 1);

        if (data) {
            strcpy(data, args[0]);
            return found_resource(data, &free);
        }
        else
            return 0;
    }
    else
        return resource_not_found();
}

static struct result* get_app(char const* path, void* value, void* data)
{
    if (value)
        return success_result(app_content, 0, 0, cookie("resource", value, 0));

    return 0;
}

static struct entity* write_app(void* data)
{
    char* body = malloc(strlen((char*)data) + 1);

    if (body) {
        strncpy(body, data, strlen((char*)data) + 1);
        return build_entity(body);
    }

    return 0;
}

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
            return success_result(body, &free, 0, 0);
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

static char* read_file(char const* path)
{
    char* buffer = 0;
    long size;
    FILE* fp = fopen(path, "rb");

    if (fp && fseek(fp, 0L, SEEK_END) == 0 && (size = ftell(fp)) != -1) {
        if (buffer = malloc(size + 1)) {
            rewind(fp);
            if (size = fread(buffer, sizeof(char), size, fp))
                buffer[size] = 0;
            else {
                free(buffer);
                buffer = 0;
            }
        }
    }

    fclose(fp);

    return buffer;
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
        path_route(route_pattern("^/app(/evens/.*)$",
            route_resource(resource_define(&find_app,
                resource_method("GET", 0, entity_writer("text/html", &write_app, 0), &get_app,
            0)),
        0)),
        path_route(route_pattern("^/evens/([0-9]+)$",
            route_resource(resource_define(&find_even,
                resource_method("GET", 0, entity_writer("application/json", &write_even, 0), &get_even,
            0)),
        0)),
    0));

    app_content = read_file("public/evens.html");

    run_app(port, routes);

    free(app_content);

    free_routes(routes);

    return 0;
}
