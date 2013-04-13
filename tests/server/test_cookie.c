#include <stdlib.h>
#include <stdio.h>

#include "CUnit/Basic.h"

#include "server/types/cookie.h"

#include "server/cookie.c"

static void test_cookie(void)
{
    char* key = "some", *value = "cookie";
    struct cookie* c = cookie(key, value, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(c);
    CU_ASSERT_STRING_EQUAL(c->name, "some");
    CU_ASSERT_STRING_EQUAL(c->value, "cookie");
    CU_ASSERT_PTR_NULL(c->next);

    free(c->name);
    free(c->value);
    free(c);
}

static void test_next(void)
{
    char* key = "some", *value = "cookie";
    struct cookie next = { 0, 0, 0 };
    struct cookie* c = cookie(key, value, &next);

    CU_ASSERT_PTR_NOT_NULL_FATAL(c);
    CU_ASSERT_PTR_EQUAL(c->next, &next);

    free(c->name);
    free(c->value);
    free(c);
}

static void test_illegal_value(void)
{
    char* key = "some", *value = "coo=kie,";
    struct cookie* c = cookie(key, value, 0);

    CU_ASSERT_PTR_NULL(c);
}

static void test_empty_value(void)
{
    char* key = "some", *value = "";
    struct cookie* c = cookie(key, value, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(c);
    CU_ASSERT_STRING_EQUAL(c->name, "some");
    CU_ASSERT_STRING_EQUAL(c->value, "");
    CU_ASSERT_PTR_NULL(c->next);

    free(c->name);
    free(c->value);
    free(c);
}

static void test_null_value(void)
{
    char* key = "some", *value = 0;
    struct cookie* c = cookie(key, value, 0);

    CU_ASSERT_PTR_NULL(c);
}

int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("cookie structure creation", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_cookie) ||
        !CU_ADD_TEST(suite, test_next) ||
        !CU_ADD_TEST(suite, test_illegal_value) ||
        !CU_ADD_TEST(suite, test_empty_value) ||
        !CU_ADD_TEST(suite, test_null_value))
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
