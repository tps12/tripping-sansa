#include <regex.h>

#include "CUnit/Basic.h"

int match_regex(char const* input, char const* pattern)
{
    regex_t regex;
    int result;

    if (!regcomp(&regex, pattern, REG_EXTENDED)) {
        result = regexec(&regex, input, 0, 0, 0);
        regfree(&regex);
        if (!result)
            return 1;
        else if (result == REG_NOMATCH)
            return 0;
        else
            CU_FAIL("Regex match errored");
    }
    else
        CU_FAIL("Regex compilation errored");
    return 0;
}
