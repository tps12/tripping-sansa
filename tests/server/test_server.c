#include <stdlib.h>

#include "CUnit/Basic.h"

#include "test_helpers.h"

#include "routing/types/route.h"
#include "server/types/resource.h"

struct path_route { };

static struct path_route* found_route;

static struct resource* resource = 0;

struct resource* route_path(struct path_route const* route, char const* path)
{
    return route == found_route ? resource : 0;
}

#include "server/server.c"

static int method_called, another_method_called, reader_called, writer_called;

static struct response* method(char const* path, void* data)
{
    method_called++;
    return 0;
}

static struct response* another_method(char const* path, void* data)
{
    another_method_called++;
    return 0;
}

static void* reader(char const* entity, size_t entity_length)
{
    reader_called++;
    return 0;
}

static struct response* writer(void* data)
{
    writer_called++;
    return 0;
}

static void test_delegating_request_to_resource(void)
{
    struct route* routes;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method("GET", 0, 0, &method, 0));

    method_called = 0;

    free_response(handle_request(routes, "GET", "something", 0, 0, 0, 0));

    CU_ASSERT_TRUE(method_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_delegating_request_to_second_route(void)
{
    struct route* routes;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(
        malloc(sizeof(struct path_route)), path_route(
        found_route, 0));

    resource = resource_define(resource_method("GET", 0, 0, &method, 0));

    method_called = 0;

    free_response(handle_request(routes, "GET", "something", 0, 0, 0, 0));

    CU_ASSERT_TRUE(method_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_calls_correct_method(void)
{
    struct route* routes;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method(
        "GET", 0, 0, &method, resource_method(
        "PUT", 0, 0, &another_method, 0)));

    method_called = another_method_called = 0;

    free_response(handle_request(routes, "PUT", "something", 0, 0, 0, 0));

    CU_ASSERT_FALSE(method_called);
    CU_ASSERT_TRUE(another_method_called);

    another_method_called = 0;

    free_response(handle_request(routes, "GET", "something", 0, 0, 0, 0));

    CU_ASSERT_TRUE(method_called);
    CU_ASSERT_FALSE(another_method_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_resource_not_found(void)
{
    struct route* routes = path_route(malloc(sizeof(struct path_route)), 0);
    struct response* response = 0;

    resource = resource_define(resource_method("GET", 0, 0, &method, 0));

    method_called = 0;

    response = handle_request(routes, "GET", "something", 0, 0, 0, 0);

    CU_ASSERT_FALSE(method_called);
    CU_ASSERT_PTR_NOT_NULL_FATAL(response);
    CU_ASSERT_EQUAL(response->status, 404);

    free_response(response);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_method_not_supported(void)
{
    struct route* routes;
    struct response* response = 0;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method(
        "GET", 0, 0, &method, resource_method(
        "PUT", 0, 0, &another_method, 0)));

    method_called = another_method_called = 0;

    response = handle_request(routes, "POST", "something", 0, 0, 0, 0);

    CU_ASSERT_FALSE(method_called);
    CU_ASSERT_FALSE(another_method_called);
    CU_ASSERT_PTR_NOT_NULL_FATAL(response);
    CU_ASSERT_NOT_EQUAL_FATAL(response->entity_length, 0);
    CU_ASSERT_STRING_MATCH(response->entity, "not allowed");
    CU_ASSERT_EQUAL(response->status, 405);
    CU_ASSERT_STRING_MATCH(response->allow, "GET");
    CU_ASSERT_STRING_MATCH(response->allow, "PUT");

    free_response(response);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_not_acceptable(void)
{
    struct route* routes;
    struct response* response = 0;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method(
        "GET", 0, entity_writer("some/provided-type", &writer, 0), &method, 0));

    method_called = writer_called = 0;

    response = handle_request(routes, "GET", "something", 0, 0, 0, "some/other-type");

    CU_ASSERT_FALSE(method_called);
    CU_ASSERT_FALSE(writer_called);
    CU_ASSERT_PTR_NOT_NULL_FATAL(response);
    CU_ASSERT_NOT_EQUAL_FATAL(response->entity_length, 0);
    CU_ASSERT_STRING_MATCH(response->entity, "some/provided-type");
    CU_ASSERT_EQUAL(response->status, 406);

    free_response(response);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_write_type(void)
{
    struct route* routes;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method(
        "GET", 0, entity_writer("some/provided-type", &writer, 0), &method, 0));

    writer_called = 0;

    free_response(handle_request(routes, "GET", "something", 0, 0, 0, "some/provided-type"));

    CU_ASSERT_TRUE(writer_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_unsupported_type(void)
{
    struct route* routes;
    struct response* response = 0;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method(
        "PUT", entity_reader("some/required-type", &reader, 0), 0, &method, 0));

    method_called = reader_called = 0;

    response = handle_request(routes, "PUT", "something", "some/other-type", "blah", 4, 0);

    CU_ASSERT_FALSE(method_called);
    CU_ASSERT_FALSE(reader_called);
    CU_ASSERT_PTR_NOT_NULL_FATAL(response);
    CU_ASSERT_NOT_EQUAL_FATAL(response->entity_length, 0);
    CU_ASSERT_STRING_MATCH(response->entity, "some/required-type");
    CU_ASSERT_EQUAL(response->status, 415);

    free_response(response);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_read_type(void)
{
    struct route* routes;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method(
        "PUT", entity_reader("some/required-type", &reader, 0), 0, &method, 0));

    reader_called = 0;

    free_response(handle_request(routes, "PUT", "something", "some/required-type", "blah", 4, 0));

    CU_ASSERT_TRUE(reader_called);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_response_without_entity(void)
{
    struct route* routes;
    struct response* response = 0;

    found_route = malloc(sizeof(struct path_route));
    routes = path_route(found_route, 0);

    resource = resource_define(resource_method("DELETE", 0, 0, &method, 0));

    method_called = 0;

    response = handle_request(routes, "DELETE", "something", 0, 0, 0, 0);
    CU_ASSERT_TRUE_FATAL(method_called);
    CU_ASSERT_PTR_NOT_NULL_FATAL(response);
    CU_ASSERT_EQUAL(response->entity_length, 0);
    CU_ASSERT_PTR_NULL(response->entity);
    CU_ASSERT_EQUAL(response->status, 204);

    free_response(response);

    free_resource(resource);
    resource = 0;

    free_routes(routes);
}

static void test_response_with_entity(void)
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
        !CU_ADD_TEST(suite, test_calls_correct_method) ||
        !CU_ADD_TEST(suite, test_resource_not_found) ||
        !CU_ADD_TEST(suite, test_method_not_supported) ||
        !CU_ADD_TEST(suite, test_unsupported_type) ||
        !CU_ADD_TEST(suite, test_read_type) ||
        !CU_ADD_TEST(suite, test_not_acceptable) ||
        !CU_ADD_TEST(suite, test_write_type) /*||
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
