#ifndef _MATH_H
#define _MATH_H 1
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

double acos(double); // todo
float acosf(float); // todo
double acosh(double);  // todo
float acoshf(float);   // todo
long double acoshl(long double);   // todo
long double acosl(long double);  // todo
double asin(double);   // todo
float asinf(float);  // todo
double asinh(double);  // todo
float asinhf(float);   // todo
long double asinhl(long double);   // todo
long double asinl(long double);  // todo
double atan(double);   // todo
double atan2(double, double);  // todo
float atan2f(float, float);  // todo
long double atan2l(long double, long double);  // todo
float atanf(float);  // todo
double atanh(double);  // todo
float atanhf(float);   // todo
long double atanhl(long double);   // todo
long double atanl(long double);  // todo
double cbrt(double);   // todo
float cbrtf(float);  // todo
long double cbrtl(long double);  // todo
double ceil(double);   // todo
float ceilf(float);  // todo
long double ceill(long double);  // todo
double copysign(double, double);   // todo
float copysignf(float, float);   // todo
long double copysignl(long double, long double);   // todo
double cos(double);  // todo
float cosf(float);   // todo
double cosh(double);   // todo
float coshf(float);  // todo
long double coshl(long double);  // todo
long double cosl(long double);   // todo
double erf(double);  // todo
double erfc(double);   // todo
float erfcf(float);  // todo
long double erfcl(long double);  // todo
float erff(float);   // todo
long double erfl(long double);   // todo
double exp(double);  // todo
double exp2(double);   // todo
float exp2f(float);  // todo
long double exp2l(long double);  // todo
float expf(float);   // todo
long double expl(long double);   // todo
double expm1(double);  // todo
float expm1f(float);   // todo
long double expm1l(long double);   // todo
double fabs(double);
float fabsf(float);
long double fabsl(long double);
double fdim(double, double);   // todo
float fdimf(float, float);   // todo
long double fdiml(long double, long double);   // todo
double floor(double);  // todo
float floorf(float);   // todo
long double floorl(long double);   // todo
double fma(double, double, double);  // todo
float fmaf(float, float, float);   // todo
long double fmal(long double, long double, long double);   // todo
double fmax(double, double);   // todo
float fmaxf(float, float);   // todo
long double fmaxl(long double, long double);   // todo
double fmin(double, double);   // todo
float fminf(float, float);   // todo
long double fminl(long double, long double);   // todo
double fmod(double, double);   // todo
float fmodf(float, float);   // todo
long double fmodl(long double, long double);   // todo
double frexp(double, int*);  // todo
float frexpf(float, int*);   // todo
long double frexpl(long double, int*);   // todo
double hypot(double, double);  // todo
float hypotf(float, float);  // todo
long double hypotl(long double, long double);  // todo
int ilogb(double);   // todo
int ilogbf(float);   // todo
int ilogbl(long double);   // todo
double j0(double);   // todo
double j1(double);   // todo
double jn(int, double);  // todo
double ldexp(double, int);   // todo
float ldexpf(float, int);  // todo
long double ldexpl(long double, int);  // todo
double lgamma(double);   // todo
float lgammaf(float);  // todo
long double lgammal(long double);  // todo
long long llrint(double);  // todo
long long llrintf(float);  // todo
long long llrintl(long double);  // todo
long long llround(double);   // todo
long long llroundf(float);   // todo
long long llroundl(long double);   // todo
double log(double);  // todo
double log10(double);  // todo
float log10f(float);   // todo
long double log10l(long double);   // todo
double log1p(double);  // todo
float log1pf(float);   // todo
long double log1pl(long double);   // todo
double log2(double);   // todo
float log2f(float);  // todo
long double log2l(long double);  // todo
double logb(double);   // todo
float logbf(float);  // todo
long double logbl(long double);  // todo
float logf(float);   // todo
long double logl(long double);   // todo
long lrint(double);  // todo
long lrintf(float);  // todo
long lrintl(long double);  // todo
long lround(double);   // todo
long lroundf(float);   // todo
long lroundl(long double);   // todo
double modf(double, double*);  // todo
float modff(float, float*);  // todo
long double modfl(long double, long double*);  // todo
double nan(const char*);   // todo
float nanf(const char*);   // todo
long double nanl(const char*);   // todo
double nearbyint(double);  // todo
float nearbyintf(float);   // todo
long double nearbyintl(long double);   // todo
double nextafter(double, double);  // todo
float nextafterf(float, float);  // todo
long double nextafterl(long double, long double);  // todo
double nexttoward(double, long double);  // todo
float nexttowardf(float, long double);   // todo
long double nexttowardl(long double, long double);   // todo
double pow(double, double);  // todo
float powf(float, float);  // todo
long double powl(long double, long double);  // todo
double remainder(double, double);  // todo
float remainderf(float, float);  // todo
long double remainderl(long double, long double);  // todo
double remquo(double, double, int*);   // todo
float remquof(float, float, int*);   // todo
long double remquol(long double, long double, int*);   // todo
double rint(double);   // todo
float rintf(float);  // todo
long double rintl(long double);  // todo
double round(double);  // todo
float roundf(float);   // todo
long double roundl(long double);   // todo
double scalbln(double, long);  // todo
float scalblnf(float, long);   // todo
long double scalblnl(long double, long);   // todo
double scalbn(double, int);  // todo
float scalbnf(float, int);   // todo
long double scalbnl(long double, int);   // todo
double sin(double);  // todo
float sinf(float);   // todo
double sinh(double);   // todo
float sinhf(float);  // todo
long double sinhl(long double);  // todo
long double sinl(long double);   // todo
double sqrt(double);
float sqrtf(float);
long double sqrtl(long double);
double tan(double);  // todo
float tanf(float);   // todo
double tanh(double);   // todo
float tanhf(float);  // todo
long double tanhl(long double);  // todo
long double tanl(long double);   // todo
double tgamma(double);   // todo
float tgammaf(float);  // todo
long double tgammal(long double);  // todo
double trunc(double);  // todo
float truncf(float);   // todo
long double truncl(long double);   // todo
double y0(double);   // todo
double y1(double);   // todo
double yn(int, double);  // todo

#endif // todo
