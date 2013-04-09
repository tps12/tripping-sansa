#include <stdlib.h>

#include "CUnit/Basic.h"

struct resource {
    int find;
};

struct found_resource { };

#include "routing/routing.c"

static char** find_args = 0;

struct found_resource* find_resource(struct resource* resource, char const* path, char const** args)
{
    static struct found_resource result;
    int i, n, l;

    if (args[0]) {
        for (n = 0; args[n]; n++)
            ;
        find_args = calloc(n + 1, sizeof(char*));
        for (i = 0; i < n; i++) {
            l = strlen(args[i]) + 1;
            find_args[i] = calloc(l, sizeof(char));
            strncpy(find_args[i], args[i], l);
        }
        find_args[i] = 0;
    }

    return resource->find ? &result : 0;
}

void free_resource(struct resource* resource) { }

static void test_finding_resource(void)
{
    struct resource resource = { 1 };
    struct path_route* route = route_pattern("some/pattern", route_resource(
        &resource, 0));

    CU_ASSERT_PTR_NOT_NULL(route_path(route, "some/pattern"));

    free_route(route);
}

static void test_finding_second_resource(void)
{
    struct resource missing_resource = { 0 };
    struct resource found_resource = { 1 };
    struct path_route* route = route_pattern("some/pattern", route_resource(
        &missing_resource, route_resource(
        &found_resource, 0)));

    CU_ASSERT_PTR_NOT_NULL(route_path(route, "some/pattern"));

    free_route(route);
}

static void test_resource_not_found(void)
{
    struct resource resource = { 0 };
    struct path_route* route = route_pattern("some/pattern", route_resource(
        &resource, 0));

    CU_ASSERT_PTR_NULL(route_path(route, "some/pattern"));

    free_route(route);
}

static void test_find_arguments(void)
{
    int i;
    struct resource resource = { 0 };
    struct path_route* route = route_pattern("some/([a-z]+)/pattern/([0-9]+)", route_resource(
        &resource, 0));

    route_path(route, "some/old/pattern/420");

    CU_ASSERT_PTR_NOT_NULL_FATAL(find_args);
    CU_ASSERT_STRING_EQUAL(find_args[0], "old");
    CU_ASSERT_STRING_EQUAL(find_args[1], "420");
    CU_ASSERT_PTR_NULL(find_args[2]);

    for (i = 0; find_args[i]; i++)
        free(find_args[i]);
    free(find_args);
    free_route(route);
}

int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("routing", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_finding_resource) ||
        !CU_ADD_TEST(suite, test_finding_second_resource) ||
        !CU_ADD_TEST(suite, test_resource_not_found) ||
        !CU_ADD_TEST(suite, test_find_arguments))
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
