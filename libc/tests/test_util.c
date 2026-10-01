#include "test_util.h"

#include <math.h>
#include <stdio.h>

static int failures;

void test_expect_true(
    bool condition, const char *expression, const char *file, int line
) {
    if (!condition) {
        printf("FAIL: %s:%d: %s\n", file, line, expression);
        failures++;
    }
}

void test_expect_close(
    double actual,
    double expected,
    const char *actual_expression,
    const char *expected_expression,
    const char *file,
    int line
) {
    double difference = fabs(actual - expected);
    double scale = fmax(fabs(actual), fabs(expected));
    bool close = difference <= 1e-12 + 1e-12 * scale;

    if (!close) {
        printf(
            "FAIL: %s:%d: %s = %.17g, expected %s = %.17g\n",
            file,
            line,
            actual_expression,
            actual,
            expected_expression,
            expected
        );
        failures++;
    }
}

int test_finish(const char *name) {
    if (failures != 0) {
        printf("%s: %d test(s) failed\n", name, failures);
        return 1;
    }

    printf("%s: all tests passed\n", name);
    return 0;
}
