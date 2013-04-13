#include <stdlib.h>

#include "CUnit/Basic.h"

#include "media_types/html.c"


int main()
{
    CU_pSuite suite = 0;

    if (CUE_SUCCESS != CU_initialize_registry())
        goto didnt_even_fail;

    suite = CU_add_suite("HTML responses", 0, 0);
    if (!suite)
        goto failed;

    if (!CU_ADD_TEST(suite, test_content) ||
        !CU_ADD_TEST(suite, test_cookie))
        goto failed;

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

failed:
    CU_cleanup_registry();
didnt_even_fail:
    return CU_get_error();
}
