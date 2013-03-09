#include <stdlib.h>

#include "CUnit/Basic.h"

typedef struct {
} resource_t;

#include "routing/routing.c"

static char** find_args = 0;

static resource_t* find_resource_with_args(char const* path, char const** args)
{
    int i, n, l;

    if (args) {
        for (n = 0; args[n]; n++)
            ;
        find_args = calloc(n + 1, sizeof(char*));
        for (i = 0; i < n; i++) {
            l = strnlen(args[i], 2048) + 1;
            find_args[i] = calloc(l, sizeof(char));
            strncpy(find_args[i], args[i], l);
        }
        find_args[i] = 0;
    }
    return 0;
}

static resource_t* find_resource(char const* path, char const** args)
{
    static resource_t result;
    return &result;
}

static resource_t* dont_find_resource(char const* path, char const** args)
{
    return 0;
}

static void test_finding_resource(void)
{
    path_route_t* route = route_pattern("some/([a-z]+)/pattern/([0-9]+)", route_resource(
        &find_resource, 0));

    CU_ASSERT_PTR_NOT_NULL(route_path(route, "some/old/pattern/420"));

    free_route(route);
}

static void test_finding_second_resource(void)
{
    path_route_t* route = route_pattern("some/([a-z]+)/pattern/([0-9]+)", route_resource(
        &dont_find_resource, route_resource(
        &find_resource, 0)));

    CU_ASSERT_PTR_NOT_NULL(route_path(route, "some/old/pattern/420"));

    free_route(route);
}

static void test_resource_not_found(void)
{
    path_route_t* route = route_pattern("some/([a-z]+)/pattern/([0-9]+)", route_resource(
        &dont_find_resource, 0));

    CU_ASSERT_PTR_NULL(route_path(route, "some/old/pattern/420"));

    free_route(route);
}

static void test_find_arguments(void)
{
    int i;
    path_route_t* route = route_pattern("some/([a-z]+)/pattern/([0-9]+)", route_resource(
        &find_resource_with_args, 0));

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

    suite = CU_add_suite("suite", 0, 0);
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
