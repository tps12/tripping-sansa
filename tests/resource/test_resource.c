#include <stdlib.h>
#include <stdio.h>

#include "CUnit/Basic.h"

#include "resource/resource.c"

struct find_result* find_result = 0;

static struct find_result* find(char const* path, char const** args)
{
    return find_result;
}

struct resource resource = { &find, 0 };

static void* free_called_with = 0;

static void free_data(void* data)
{
    free_called_with = data;
}

static void test_find_resource_errored(void)
{
    find_result = 0;

    CU_ASSERT_PTR_NULL(find_resource(&resource, 0, 0));
}

static void test_find_resource_failed(void)
{
    struct found_resource* found = find_resource(&resource, 0, 0);

    CU_ASSERT_PTR_NULL(found);
}

static void test_find_resource_succeeded(void)
{
    struct found_resource* found = 0;

    find_result = malloc(sizeof(struct find_result));
    find_result->found = 1;

    found = find_resource(&resource, 0, 0);

    CU_ASSERT_PTR_NOT_NULL(found);
    CU_ASSERT_PTR_EQUAL(found->resource, &resource);

    free(found);
}

static void test_not_found(void)
{
    struct find_result* result = 0;

    result = resource_not_found();

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_FALSE(result->found);
    CU_ASSERT_PTR_NULL(result->data);
    CU_ASSERT_PTR_NULL(result->free_data);

    free_find_result(result);
}

static void test_found_without_data(void)
{
    struct find_result* result = 0;

    result = found_resource(0, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_TRUE(result->found);
    CU_ASSERT_PTR_NULL(result->data);
    CU_ASSERT_PTR_NULL(result->free_data);

    free_find_result(result);
}

static void test_found_with_data(void)
{
    char* data = malloc(32);
    struct find_result* result = 0;

    sprintf(data, "some entity data");

    result = found_resource((void*)data, &free_data);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_TRUE(result->found);
    CU_ASSERT_PTR_EQUAL(result->data, data);
    CU_ASSERT_PTR_EQUAL(result->free_data, &free_data);

    free_find_result(result);
    free(data);
}

static void test_free_without_free_data(void)
{
    char data[] = "some data";
    struct find_result* result = 0;

    free_called_with = 0;

    result = found_resource((void*)data, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);

    free_find_result(result);
    CU_ASSERT_PTR_NULL(free_called_with);
}

static void test_free_with_free_data(void)
{
    char* data = malloc(32);
    sprintf(data, "some entity data");

    free_called_with = 0;

    free_find_result(found_resource((void*)data, &free_data));
    CU_ASSERT_PTR_NULL(free_called_with);

    free(data);
}

int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("resource functions", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_find_resource_errored) ||
        !CU_ADD_TEST(suite, test_find_resource_failed) ||
        !CU_ADD_TEST(suite, test_find_resource_succeeded) ||
        !CU_ADD_TEST(suite, test_not_found) ||
        !CU_ADD_TEST(suite, test_found_without_data) ||
        !CU_ADD_TEST(suite, test_found_with_data) ||
        !CU_ADD_TEST(suite, test_free_with_free_data) ||
        !CU_ADD_TEST(suite, test_free_without_free_data))
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
