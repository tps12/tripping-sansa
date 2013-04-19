#include <stdlib.h>
#include <stdio.h>

#include "CUnit/Basic.h"

#include "db/types/mongo_host.h"

#include "db/mongo.c"

static void test_uri_without_port()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://localhost");

    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_PTR_NULL(hosts->next);
    CU_ASSERT_STRING_EQUAL(hosts->host, "localhost");
    CU_ASSERT_EQUAL(hosts->port, 27017);

    free_hosts(hosts);
}

static void test_basic_uri()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://localhost:27018");

    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_PTR_NULL(hosts->next);
    CU_ASSERT_STRING_EQUAL(hosts->host, "localhost");
    CU_ASSERT_EQUAL(hosts->port, 27018);

    free_hosts(hosts);
}

static void test_multiple_uris()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://a.example.com:27018,b.example.com");

    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_STRING_EQUAL(hosts->host, "a.example.com");
    CU_ASSERT_EQUAL(hosts->port, 27018);

    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts->next);
    CU_ASSERT_STRING_EQUAL(hosts->next->host, "b.example.com");
    CU_ASSERT_EQUAL(hosts->next->port, 27017);

    CU_ASSERT_PTR_NULL(hosts->next->next);

    free_hosts(hosts);
}

static void test_complex_passwords()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://bob:secret.word@a.example.com:27018/test");

    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_STRING_EQUAL(hosts->username, "bob");
    CU_ASSERT_STRING_EQUAL(hosts->password, "secret.word");
    free_hosts(hosts);

    hosts = parse_mongo_uri("mongodb://bob:s-_3#%R.t@a.example.com:27018/test");
    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_STRING_EQUAL(hosts->username, "bob");
    CU_ASSERT_STRING_EQUAL(hosts->password, "s-_3#%R.t");
    free_hosts(hosts);
}

static void test_complex_usernames()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://b:ob:secret.word@a.example.com:27018/test");
    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_STRING_EQUAL(hosts->username, "b:ob");
    free_hosts(hosts);
}

static void test_passwords_contain_no_commas()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://bob:a,b@a.example.com:27018/test");

    CU_ASSERT_PTR_NULL(hosts);
}

static void test_multiple_uris_with_auths()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://bob:secret@a.example.com:27018,b.example.com/test");
    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts);
    CU_ASSERT_STRING_EQUAL(hosts->host, "a.example.com");
    CU_ASSERT_EQUAL(hosts->port, 27018);
    CU_ASSERT_STRING_EQUAL(hosts->username, "bob");
    CU_ASSERT_STRING_EQUAL(hosts->password, "secret");
    CU_ASSERT_STRING_EQUAL(hosts->database, "test");

    CU_ASSERT_PTR_NOT_NULL_FATAL(hosts->next);
    CU_ASSERT_STRING_EQUAL(hosts->next->host, "b.example.com");
    CU_ASSERT_EQUAL(hosts->next->port, 27017);
    CU_ASSERT_STRING_EQUAL(hosts->next->username, "bob");
    CU_ASSERT_STRING_EQUAL(hosts->next->password, "secret");
    CU_ASSERT_STRING_EQUAL(hosts->next->database, "test");

    CU_ASSERT_PTR_NULL(hosts->next->next);

    free_hosts(hosts);
}

static void test_opts_not_supported()
{
    struct mongo_host* hosts = parse_mongo_uri("mongodb://localhost:27018?connect=direct;slaveok=true;safe=true");

    CU_ASSERT_PTR_NULL(hosts);
}

int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("mongodb:// URI parsing", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_uri_without_port) ||
        !CU_ADD_TEST(suite, test_basic_uri) ||
        !CU_ADD_TEST(suite, test_multiple_uris) ||
        !CU_ADD_TEST(suite, test_complex_passwords) ||
        !CU_ADD_TEST(suite, test_complex_usernames) ||
        !CU_ADD_TEST(suite, test_passwords_contain_no_commas) ||
        !CU_ADD_TEST(suite, test_multiple_uris_with_auths) ||
        !CU_ADD_TEST(suite, test_opts_not_supported))
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
