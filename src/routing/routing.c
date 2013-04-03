#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "routing/types/find_resource_fn.h"
#include "routing/types/route.h"

struct route_finder {
    find_resource_fn finder;
    struct route_finder* next;
};

struct path_route {
    regex_t regex;
    struct route_finder* finders;
};

struct path_route* route_pattern(char const* pattern, struct route_finder* finders)
{
    int result;

    struct path_route* route = malloc(sizeof(struct path_route));
    if (route) {
        result = regcomp(&route->regex, pattern, REG_EXTENDED);
        if (result) {
            free(route);
            route = 0;
        }
        else
            route->finders = finders;
    }
    return route;
}

struct route_finder* route_resource(find_resource_fn find_resource, struct route_finder* next)
{
    struct route_finder* result = malloc(sizeof(struct route_finder));
    if (result) {
        result->finder = find_resource;
        result->next = next;
    }
    return result;
}

struct resource* route_path(struct path_route const* route, char const* path)
{
    int i, len, match_count, failed;
    regmatch_t* matches = 0;
    char** args = 0;
    struct route_finder* finder;
    struct resource* resource = 0;

    match_count = 1 + route->regex.re_nsub;
    matches = calloc(match_count, sizeof(regmatch_t));
    if (!matches)
        return 0;

    if (!regexec(&route->regex, path, match_count, matches, 0)) {
        failed = 0;
        args = calloc(match_count, sizeof(char*));
        if (!args)
            failed = 1;
        else {
            for (i = 0; i < match_count - 1; i++) {
                len = matches[i+1].rm_eo - matches[i+1].rm_so;
                args[i] = calloc(len + 1, sizeof(char));
                if (!args[i]) {
                    failed = 1;
                    break;
                }
                strncpy(args[i], path + matches[i+1].rm_so, len);
                args[i][len] = 0;
            }
            args[i] = 0;
        }

        if (!failed) {
            for (finder = route->finders; finder; finder = finder->next)
                if (resource = finder->finder(path, (char const**)args))
                    break;
            for (i = 0; i < match_count; i++)
                free(args[i]);
            free(args);
        }
    }

    free(matches);
    return resource;
}

static void free_finders(struct route_finder* finder)
{
    if (finder)
        free_finders(finder->next);
    free(finder);
}

void free_route(struct path_route* route)
{
    regfree(&route->regex);
    free_finders(route->finders);
    free(route);
}
