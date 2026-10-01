#include "../test_util.h"
#include <math.h>

static void test_reference_values(void) {
    expect_close(log(0.125), logl(0.125L));
    expect_close(log(0.999), logl(0.999L));
    expect_close(log(1.0), logl(1.0L));
    expect_close(log(1.25), logl(1.25L));
    expect_close(log(2.0), logl(2.0L));
    expect_close(log(10.0), logl(10.0L));
    expect_close(log(1000.0), logl(1000.0L));
    expect_close(log(0x1p-10), logl(0x1p-10L));
    expect_close(log(0x1.fffffffffffffp+10), logl(0x1.fffffffffffffp+10L));
}

static void test_log_identities(void) {
    double product = 1.25 * 8.0;
    double expected = logl(1.25L) + logl(8.0L);

    expect_close(log(product), expected);

    expect_true(log(1.0) == 0.0);
}

static void test_special_values(void) {
    double negative_zero = -0.0;

    expect_true(isinf(log(0.0)) && signbit(log(0.0)));

    expect_true(isinf(log(negative_zero)) && signbit(log(negative_zero)));

    expect_true(isnan(log(-1.0)));
    expect_true(isinf(log(INFINITY)) && !signbit(log(INFINITY)));
    expect_true(isnan(log(NAN)));
}

int main(void) {
    test_reference_values();
    test_log_identities();
    test_special_values();

    return test_finish("log");
}
