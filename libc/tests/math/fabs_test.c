#include "../test_util.h"
#include <math.h>

static void test_double_values(void) {
    expect_true(fabs(3.5) == 3.5);
    expect_true(fabs(-3.5) == 3.5);
    expect_true(fabs(0.0) == 0.0 && !signbit(fabs(0.0)));
    expect_true(fabs(-0.0) == 0.0 && !signbit(fabs(-0.0)));
    expect_true(isinf(fabs(INFINITY)) && !signbit(fabs(INFINITY)));
    expect_true(isinf(fabs(-INFINITY)) && !signbit(fabs(-INFINITY)));
    expect_true(isnan(fabs(NAN)));
}

static void test_float_values(void) {
    expect_true(fabsf(-3.5f) == 3.5f);
    expect_true(fabsf(-0.0f) == 0.0f && !signbit(fabsf(-0.0f)));
    expect_true(isnan(fabsf(NAN)));
}

static void test_long_double_values(void) {
    expect_true(fabsl(-3.5L) == 3.5L);
    expect_true(fabsl(-0.0L) == 0.0L && !signbit(fabsl(-0.0L)));
    expect_true(isnan(fabsl(NAN)));
}

int main(void) {
    test_double_values();
    test_float_values();
    test_long_double_values();

    return test_finish("fabs");
}
