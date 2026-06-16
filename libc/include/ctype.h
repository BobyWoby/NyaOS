#ifndef _CTYPE_H
#define _CTYPE_H 1

#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int isdigit(int c);
int isxdigit(int c);

#ifdef __cplusplus
}
#endif

#endif
