#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define LN2 0.69314718055994530942
#define LOG2E 1.44269504088896340736
#define LOG10E 0.43429448190325182765
#define LOG10_2 0.30102999566398119521
#define SQRT2 1.41421356237309504880

double atanh_series(double);

static double reduce(double x, int *pe) {
    uint64_t u;
    memcpy(&u, &x, sizeof(u));

    int e = (int)((u >> 52) & 0x7FF);

    if (e == 0) {
        x *= 0x1p54;
        memcpy(&u, &x, sizeof(u));
        e = (int)((u >> 52) & 0x7FF) - 54;
    }
    e -= 1023;

    u = (u & 0x000FFFFFFFFFFFFFull) | ((uint64_t)1023 << 52);

    double m;
    memcpy(&m, &u, sizeof(m));

    if (m > SQRT2) {
        m *= 0.5;
        e += 1;
    }

    *pe = e;
    return m;
}

static double ln_mantissa(double m) {
    return 2.0 * atanh_series((m - 1.0) / (m + 1.0));
}

#define LOG_GUARDS(x)                                                          \
    do {                                                                       \
        if (__builtin_isnan(x))                                                \
            return (x);                                                        \
        if ((x) == 0.0)                                                        \
            return -__builtin_inf();                                           \
        if ((x) < 0.0)                                                         \
            return __builtin_nan("");                                          \
        if (__builtin_isinf(x))                                                \
            return __builtin_inf();                                            \
    } while (0)

double log(double x) {
    LOG_GUARDS(x);

    int e;
    double m = reduce(x, &e);

    return ln_mantissa(m) + (double)e * LN2;
}

double log2(double x) {
    LOG_GUARDS(x);

    int e;
    double m = reduce(x, &e);

    return ln_mantissa(m) * LOG2E + (double)e;
}

double log10(double x) {
    LOG_GUARDS(x);

    int e;
    double m = reduce(x, &e);

    return ln_mantissa(m) * LOG10E + (double)e * LOG10_2;
}

double log1p(double x) {
    if (__builtin_isnan(x))
        return x;
    if (x == -1.0)
        return -__builtin_inf();
    if (x < -1.0)
        return __builtin_nan("");
    if (__builtin_isinf(x))
        return __builtin_inf();
    if (x == 0.0)
        return x;

    if (fabs(x) < 0.25)
        return 2.0 * atanh_series(x / (2.0 + x));

    return log(1.0 + x);
}

double logb(double x) {
    if (__builtin_isnan(x))
        return x;
    if (x == 0.0)
        return -__builtin_inf();
    if (__builtin_isinf(x))
        return __builtin_inf();

    uint64_t u;
    memcpy(&u, &x, sizeof(u));

    int e = (int)((u >> 52) & 0x7FF);

    if (e == 0) {
        u &= 0x000FFFFFFFFFFFFFull;
        return (double)(-1022 - (__builtin_clzll(u) - 11));
    }

    return (double)(e - 1023);
}

int ilogb(double x) {
    if (__builtin_isnan(x))
        return INT_MAX;
    if (x == 0.0)
        return INT_MIN;
    if (__builtin_isinf(x))
        return INT_MAX;

    return (int)logb(x);
}

float logf(float x) {
    return (float)log((double)x);
}

float log2f(float x) {
    return (float)log2((double)x);
}

float log10f(float x) {
    return (float)log10((double)x);
}

float log1pf(float x) {
    return (float)log1p((double)x);
}

float logbf(float x) {
    return (float)logb((double)x);
}

int ilogbf(float x) {
    return ilogb((double)x);
}
