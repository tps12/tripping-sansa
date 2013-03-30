#include <stdlib.h>
#include <stdio.h>

#include "CUnit/Basic.h"

#include "server/result.c"

static void* free_called_with = 0;

static void free_data(void* data)
{
    free_called_with = data;
}

static void test_error(void)
{
    char* error = malloc(32);
    struct result* result = 0;

    sprintf(error, "some error message");

    result = error_result(error);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_STRING_EQUAL(result->error, "some error message");
    CU_ASSERT_PTR_NULL(result->data);
    CU_ASSERT_PTR_NULL(result->free_data);
    CU_ASSERT_PTR_NULL(result->location);

    free_result(result);
}

static void test_blank_success(void)
{
    struct result* result = 0;

    result = success_result(0, 0, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_PTR_NULL(result->error);
    CU_ASSERT_PTR_NULL(result->data);
    CU_ASSERT_PTR_NULL(result->free_data);
    CU_ASSERT_PTR_NULL(result->location);

    free_result(result);
}

static void test_success_with_entity(void)
{
    char* data = malloc(32);
    struct result* result = 0;

    sprintf(data, "some entity data");

    result = success_result((void*)data, &free_data, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_PTR_NULL(result->error);
    CU_ASSERT_PTR_EQUAL(result->data, data);
    CU_ASSERT_PTR_EQUAL(result->free_data, &free_data);
    CU_ASSERT_PTR_NULL(result->location);

    free_result(result);
    free(data);
}

static void test_entity_without_free(void)
{
    char* data = malloc(32);
    sprintf(data, "some entity data");

    CU_ASSERT_PTR_NULL(success_result((void*)data, 0, 0));

    free(data);
}

static void test_free_result(void)
{
    char* data = malloc(32);
    sprintf(data, "some entity data");

    free_called_with = 0;

    free_result(success_result((void*)data, &free_data, 0));
    CU_ASSERT_PTR_EQUAL(free_called_with, data);

    free(data);
}

static void test_success_with_location(void)
{
    char* location = malloc(32);
    struct result* result = 0;

    sprintf(location, "some location");

    result = success_result(0, 0, location);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_PTR_NULL(result->error);
    CU_ASSERT_PTR_NULL(result->data);
    CU_ASSERT_PTR_NULL(result->free_data);
    CU_ASSERT_STRING_EQUAL(result->location, location);

    free_result(result);
}

static void test_success_with_entity_and_location(void)
{
    char* data = malloc(32);
    char* location = malloc(32);
    struct result* result = 0;

    sprintf(data, "some entity data");
    sprintf(location, "some location");

    result = success_result((void*)data, &free_data, location);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_PTR_NULL(result->error);
    CU_ASSERT_PTR_EQUAL(result->data, data);
    CU_ASSERT_PTR_EQUAL(result->free_data, &free_data);
    CU_ASSERT_STRING_EQUAL(result->location, location);

    free_result(result);
    free(data);
}

int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("result functions", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_error) ||
        !CU_ADD_TEST(suite, test_blank_success) ||
        !CU_ADD_TEST(suite, test_success_with_entity) ||
        !CU_ADD_TEST(suite, test_entity_without_free) ||
        !CU_ADD_TEST(suite, test_free_result) ||
        !CU_ADD_TEST(suite, test_success_with_location) ||
        !CU_ADD_TEST(suite, test_success_with_entity_and_location))
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
