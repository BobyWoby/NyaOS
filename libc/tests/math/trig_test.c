#include "../test_util.h"

#include <math.h>

double atanh_series(double);

int main(void) {
    expect_close(atanh_series(0.0), 0.0);
    expect_close(atanh_series(0.1), atanh(0.1));
    expect_close(atanh_series(-0.1), atanh(-0.1));
    expect_close(atanh_series(0.5), atanh(0.5));
    expect_close(atanh_series(-0.5), atanh(-0.5));

    return test_finish("trig");
}
