#include <ctype.h>

int isxdigit(int c){
    if('0' <= c && c <= '9') return true;
    if(c >= 'a' && c <= 'f') return true;
    return c >= 'A' && c <= 'F';
}
