#include <regex.h>
#include <stdio.h>

#include "routing/types/route.h"

typedef resource_t* (*find_resource_fn)(char const* path, char const** args);

typedef struct route_finder_t {
    find_resource_fn finder;
    struct route_finder_t* next;
} route_finder_t;

typedef struct {
    regex_t regex;
    route_finder_t* finders;
} path_route_t;

path_route_t* route_pattern(char const* pattern, route_finder_t* finders)
{
    int result;

    path_route_t* route = malloc(sizeof(path_route_t));
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

route_finder_t* route_resource(find_resource_fn find_resource, route_finder_t* next)
{
    route_finder_t* result = malloc(sizeof(route_finder_t));
    if (result) {
        result->finder = find_resource;
        result->next = next;
    }
    return result;
}

resource_t* route_path(path_route_t const* route, char const* path)
{
    int i, len, match_count, failed;
    regmatch_t* matches = 0;
    char** args = 0;
    route_finder_t* finder;
    resource_t* resource = 0;

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

static void free_finders(route_finder_t* finder)
{
    if (finder)
        free_finders(finder->next);
    free(finder);
}

void free_route(path_route_t* route)
{
    regfree(&route->regex);
    free_finders(route->finders);
    free(route);
}
