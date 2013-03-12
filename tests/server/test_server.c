#include <stdlib.h>

#include "CUnit/Basic.h"

#include "routing/types/route.h"
#include "server/types/resource.h"

typedef struct path_route_t {
} path_route_t;

static path_route_t found_route;

static resource_t* resource = 0;

resource_t* route_path(path_route_t const* route, char const* path)
{
    return route == &found_route ? resource : 0;
}

#include "server/server.c"

static int method_called, another_method_called;

static response_t* method(char const* path, char const* entity_type, char const* entity, size_t entity_length)
{
    method_called = 1;
    return 0;
}

static response_t* another_method(char const* path, char const* entity_type, char const* entity, size_t entity_length)
{
    another_method_called = 1;
    return 0;
}

static void test_delegating_request_to_resource(void)
{
    route_t* routes = path_route(&found_route, 0);

    resource = resource_define(resource_method("GET", &method, 0));

    method_called = 0;

    free(handle_request(routes, "GET", "something", 0, 0, 0));

    CU_ASSERT_TRUE(method_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_delegating_request_to_second_route(void)
{
    route_t* routes = path_route(
        malloc(sizeof(path_route_t)), path_route(
        &found_route, 0));

    resource = resource_define(resource_method("GET", &method, 0));

    method_called = 0;

    free(handle_request(routes, "GET", "something", 0, 0, 0));

    CU_ASSERT_TRUE(method_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_calls_correct_method(void)
{
    route_t* routes = path_route(&found_route, 0);

    resource = resource_define(resource_method(
        "GET", &method, resource_method(
        "PUT", &another_method, 0)));

    method_called = another_method_called = 0;

    free(handle_request(routes, "PUT", "something", 0, 0, 0));

    CU_ASSERT_FALSE(method_called);
    CU_ASSERT_TRUE(another_method_called);

    another_method_called = 0;

    free(handle_request(routes, "GET", "something", 0, 0, 0));

    CU_ASSERT_TRUE(method_called);
    CU_ASSERT_FALSE(another_method_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_resource_not_found(void)
{
}

static void test_method_not_supported(void)
{
}

static void test_not_acceptable(void)
{
}

static void test_unsupported_type(void)
{
}

static void test_response_status(void)
{
}

static void test_response_entity(void)
{
}

static void test_response_location(void)
{
}

static void test_responses_with_bad_status(void)
{
}

static void test_responses_without_required_entity(void)
{
}

static void test_responses_without_optional_entity(void)
{
}

int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("suite", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_delegating_request_to_resource) ||
        !CU_ADD_TEST(suite, test_delegating_request_to_second_route) ||
        !CU_ADD_TEST(suite, test_calls_correct_method) /*||
        !CU_ADD_TEST(suite, test_get_resource) ||
        !CU_ADD_TEST(suite, test_post_resource) ||
        !CU_ADD_TEST(suite, test_put_resource) ||
        !CU_ADD_TEST(suite, test_delete_resource)*/)
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
