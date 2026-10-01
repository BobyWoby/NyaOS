#include <math.h>

double atanh_series(double x) {
    // btw ts not in header file; but adding it here for log impl
    double sum = x;
    double power = x;

    const double epsilon = 1e-16;

    for (int i = 0; i < 1000; ++i) {
        power *= x * x;
        double term = power / (2 * i + 3);
        sum += term;

        if (fabs(term) < epsilon)
            break;
    }

    return sum;
}
