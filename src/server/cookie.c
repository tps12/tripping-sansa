#include <stdlib.h>
#include <string.h>

#include "server/types/cookie.h"

struct cookie* cookie(char* name, char* value, struct cookie* next)
{
    struct cookie* cookie = 0;

    if (!(name && value))
        return 0;

    if (strchr(value, '=') || strchr(value, ',') || strchr(value, ';'))
        return 0;

    cookie = malloc(sizeof(struct cookie));

    if (cookie) {
        cookie->name = malloc(strlen(name) + 1);
        cookie->value = malloc(strlen(value) + 1);
        if (cookie->name && cookie->value) {
            strcpy(cookie->name, name);
            strcpy(cookie->value, value);
            cookie->next = next;
        }
        else {
            free(cookie->name);
            free(cookie);
            cookie = 0;
        }
    }

    return cookie;
}
