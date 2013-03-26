extern int match_regex(char const* input, char const* pattern);

/** Asserts that string actual does not match expected regex.
 * Reports failure with no other action.
 */
#define CU_ASSERT_STRING_MATCH(actual, expected) \
  { CU_assertImplementation((match_regex((const char*)(actual), (const char*)(expected))), __LINE__, ("CU_ASSERT_STRING_MATCH(" #actual ","  #expected ")"), __FILE__, "", 0); }

/** Asserts that string actual does not match expected regex.
 * Reports failure and causes test to abort.
 */
#define CU_ASSERT_STRING_MATCH_FATAL(actual, expected) \
  { CU_assertImplementation((match_regex((const char*)(actual), (const char*)(expected))), __LINE__, ("CU_ASSERT_STRING_MATCH_FATAL(" #actual ","  #expected ")"), __FILE__, "", 1); }
