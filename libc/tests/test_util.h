#ifndef NYAOS_TEST_UTIL_H
#define NYAOS_TEST_UTIL_H

#include <stdbool.h>

void test_expect_true(
    bool condition, const char *expression, const char *file, int line
);

void test_expect_close(
    double actual,
    double expected,
    const char *actual_expression,
    const char *expected_expression,
    const char *file,
    int line
);

int test_finish(const char *name);

#define expect_true(expression)                                                \
    test_expect_true((expression), #expression, __FILE__, __LINE__)

#define expect_close(actual, expected)                                         \
    test_expect_close(                                                         \
        (actual), (expected), #actual, #expected, __FILE__, __LINE__           \
    )

#endif
