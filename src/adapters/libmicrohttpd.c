/*
     This file is part of libmicrohttpd
     (C) 2007 Christian Grothoff (and other contributing authors)

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
/**
 * @file minimal_example.c
 * @brief minimal example for how to use libmicrohttpd
 * @author Christian Grothoff
 */

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>

#include "server/functions/handle_request.h"
#include "server/types/cookie.h"
#include "server/types/response.h"

struct request_entity {
    void* data;
    size_t length;
};

static void add_cookies(struct MHD_Response* mhd_response, struct cookie* cookie)
{
    char* pair;

    if (cookie && (pair = malloc(strlen(cookie->name) + strlen(cookie->value) + 2))) {
        strcpy(pair, cookie->name);
        strcat(pair, "=");
        strcat(pair, cookie->value);
        MHD_add_response_header(mhd_response, "Set-Cookie", pair);
        free(pair);
        add_cookies(mhd_response, cookie->next);
    }
}

static int access_handler(void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data, size_t *upload_data_size, void **ptr)
{
    struct route* routes = cls;
    struct request_entity* entity;
    struct response* response;
    struct MHD_Response* mhd_response;
    int ret;

    if (!*ptr) {
        entity = malloc(sizeof(struct request_entity));
        entity->data = 0;
        entity->length = 0;
        *ptr = entity;
        return MHD_YES;
    }
    else
    {
        entity = *ptr;
        if (*upload_data_size) {
            entity->data = (char*)upload_data;
            entity->length = *upload_data_size;
            *upload_data_size = 0;
            return MHD_YES;
        }
        else {
            response = handle_request(
                routes,
                method,
                url,
                MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Content-Type"),
                entity->data,
                entity->length,
                MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Accept"));

            mhd_response = MHD_create_response_from_buffer(
                response->entity_length,
                (void*)response->entity,
                MHD_RESPMEM_MUST_COPY);

            if (response->allow)
                MHD_add_response_header(mhd_response, "Allow", response->allow);
            if (response->location)
                MHD_add_response_header(mhd_response, "Location", response->location);
            if (response->entity_type)
                MHD_add_response_header(mhd_response, "Content-Type", response->entity_type);

            add_cookies(mhd_response, response->cookies);

            ret = MHD_queue_response(connection, response->status, mhd_response);

            MHD_destroy_response(mhd_response);
            free(entity);
            free_response(response);

            return ret;
        }
    }
}

static int finished;

static void terminate(int signum)
{
    finished = 1;
}

void run_app(int port, struct route* routes)
{
    struct MHD_Daemon *d = MHD_start_daemon (/* MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_POLL, */
                                              MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG,
                                              /* MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG | MHD_USE_POLL,
                                              // MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG, */
                                              port,
                                              0, 0, &access_handler, routes,
                                              MHD_OPTION_CONNECTION_TIMEOUT, (unsigned int) 120,
                                              MHD_OPTION_END);

    if (d) {
        finished = 0;
        signal(SIGTERM, &terminate);

        while (!finished)
          sleep(1);

        MHD_stop_daemon(d);
    }
}
