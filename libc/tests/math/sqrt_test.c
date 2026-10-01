#include "../test_util.h"

#include <math.h>

int main(void) {
    expect_close(sqrt(0.0), 0.0);
    expect_close(sqrt(1.0), 1.0);
    expect_close(sqrt(4.0), 2.0);
    expect_close(sqrt(2.0), 1.4142135623730950488);
    expect_true(isnan(sqrt(-1.0)));
    expect_true(isinf(sqrt(INFINITY)) && !signbit(sqrt(INFINITY)));

    expect_close((double)sqrtf(0.25f), 0.5);
    expect_close((double)sqrtf(2.0f), (double)(float)1.4142135623730950488);
    expect_close((double)sqrtl(9.0L), 3.0);

    return test_finish("sqrt");
}
