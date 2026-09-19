#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int quot; /* Quotient */
    int rem;  /* Remainder */
} div_t;

typedef struct {
    long quot; /* Quotient */
    long rem;  /* Remainder */
} ldiv_t;

typedef struct {
    long long quot; /* Quotient */
    long long rem;  /* Remainder */
} lldiv_t;

typedef struct {
    intmax_t quot; /* Quotient */
    intmax_t rem;  /* Remainder */
} imaxdiv_t;

char* itoa(int num);
void _Exit(int);
long a64l(const char*);
__attribute__((__noreturn__)) void abort(void);
int abs(int);
int atexit(void (*)(void));
double atof(const char*);
int atoi(const char*);
long atol(const char*);
long long atoll(const char*);
void* bsearch(const void*, const void*, size_t, size_t, int (*)(const void*, const void*));
void* calloc(size_t, size_t);
div_t div(int, int);
double drand48(void);
double erand48(unsigned short[3]);
void exit(int);
void free(void*);
char* getenv(const char*);
int getsubopt(char**, char* const*, char**);
int grantpt(int);
char* initstate(unsigned, char*, size_t);
long jrand48(unsigned short[3]);
char* l64a(long);
long labs(long);
void lcong48(unsigned short[7]);
ldiv_t ldiv(long, long);
long long llabs(long long);
lldiv_t lldiv(long long, long long);
long lrand48(void);
void* malloc(size_t);
int mblen(const char*, size_t);
size_t mbstowcs(wchar_t* restrict, const char* restrict, size_t);
int mbtowc(wchar_t* restrict, const char* restrict, size_t);
char* mkdtemp(char*);
int mkstemp(char*);
long mrand48(void);
long nrand48(unsigned short[3]);
int posix_memalign(void**, size_t, size_t);
int posix_openpt(int);
char* ptsname(int);
int putenv(char*);
void qsort(void*, size_t, size_t, int (*)(const void*, const void*));
int rand(void);
int rand_r(unsigned*);
long random(void);
void* realloc(void*, size_t);
char* realpath(const char* restrict, char* restrict);
unsigned short* seed48(unsigned short[3]);
int setenv(const char*, const char*, int);
void setkey(const char*);
char* setstate(char*);
void srand(unsigned);
void srand48(long);
void srandom(unsigned);
double strtod(const char* restrict, char** restrict);
float strtof(const char* restrict, char** restrict);
long strtol(const char* restrict, char** restrict, int);
long double strtold(const char* restrict, char** restrict);
long long strtoll(const char* restrict, char** restrict, int);
unsigned long strtoul(const char* restrict, char** restrict, int);
unsigned long long strtoull(const char* restrict, char** restrict, int);
int system(const char*);
int unlockpt(int);
int unsetenv(const char*);
size_t wcstombs(char* restrict, const wchar_t* restrict, size_t);
int wctomb(char*, wchar_t);

#ifdef __cplusplus
}
#endif

#endif
