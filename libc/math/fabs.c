double fabs(double x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_fabs(x);
#else
    return x < 0.0 ? -x : x;
#endif
}

float fabsf(float x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_fabsf(x);
#else
    return x < 0.0f ? -x : x;
#endif
}

long double fabsl(long double x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_fabsl(x);
#else
    return x < 0.0L ? -x : x;
#endif
}
