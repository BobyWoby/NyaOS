#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < length; ++i) {
        if (putchar(bytes[i]) == EOF) return false;
    }
    return true;
}

char* __int_str(intmax_t i, char b[], int base, bool plusSignIfNeeded, bool spaceSignIfNeeded,
                int paddingNo, bool justify, bool zeroPad, int* length) {
    *length = 0;
    char digit[32] = {0};
    memset(digit, 0, 32);
    strcpy(digit, "0123456789");
    if (base == 16) {
        strcat(digit, "ABCDEF");
    } else if (base == 17) {
        strcat(digit, "abcdef");
        base = 16;
    }

    char* p = b;
    if (i < 0) {
        *p++ = '-';
        i *= -1;
    } else if (plusSignIfNeeded) {
        *p++ = '+';
    } else if (spaceSignIfNeeded) {
        *p++ = ' ';
    }

    intmax_t shifter = i;
    do {
        ++p;
        shifter = shifter / base;
        length++;
    } while (shifter);

    *p = '\0';
    do {
        *--p = digit[i % base];
        i = i / base;
    } while (i);

    int padding = paddingNo - (int)strlen(b);
    if (padding < 0) padding = 0;

    if (justify) {
        while (padding--) {
            if (zeroPad) {
                b[strlen(b)] = '0';
            } else {
                b[strlen(b)] = ' ';
            }
        }

    } else {
        char a[256] = {0};
        while (padding--) {
            if (zeroPad) {
                a[strlen(a)] = '0';
            } else {
                a[strlen(a)] = ' ';
            }
        }
        strcat(a, b);
        strcpy(b, a);
    }

    return b;
}

int vprintf(const char* format, va_list list) {
    int chars = 0;
    char intStrBuffer[256] = {0};
    for (int i = 0; format[i]; ++i) {
        char specifier = '\0';
        char length = '\0';
        int lengthSpec = 0;
        int precSpec = 0;
        bool leftJustify = false;
        bool zeroPad = false;
        bool spaceNoSign = false;
        bool altForm = false;
        bool plusSign = false;
        bool emode = false;
        int expo = 0;

        if (format[i] == '%') {
            ++i;
            bool extBreak = false;
            while (true) {
                switch (format[i]) {
                    case '-':
                        leftJustify = true;
                        ++i;
                        break;
                    case '+':
                        plusSign = true;
                        ++i;
                        break;
                    case '#':
                        altForm = true;
                        ++i;
                        break;
                    case ' ':
                        spaceNoSign = true;
                        ++i;
                        break;
                    case '0':
                        zeroPad = true;
                        break;
                    default:
                        extBreak = true;
                }
                if (extBreak) break;
            }
        }

        while (isdigit(format[i])) {
            lengthSpec *= 10;
            lengthSpec += format[i] - '0';
            ++i;
        }

        if (format[i] == '*') {
            lengthSpec = va_arg(list, int);
            ++i;
        }

        if (format[i] == '.') {
            ++i;
            while (isdigit(format[i])) {
                precSpec *= 10;
                precSpec += format[i] - '0';
                ++i;
            }
            if (format[i] == '*') {
                precSpec = va_arg(list, int);
                ++i;
            }
        } else {
            precSpec = 6;
        }

        if (format[i] == 'h' || format[i] == 'l' || format[i] == 'j' || format[i] == 'z' ||
            format[i] == 't' || format[i] == 'L') {
            length = format[i];
            ++i;
            if (format[i] == 'h') {
                length = 'H';
            } else if (format[i] == 'l') {
                length = 'q';
                ++i;
            }
        }

        specifier = format[i];
        memset(intStrBuffer, 0, 256);
        int base = 10;
        if (specifier == 'o') {
            base = 8;
            specifier = 'u';
            if (altForm) {
                if (print("0", 1)) ++chars;
                // TODO: Add smt if the print fails
            }
        }
        if (specifier == 'p') {
            base = 16;
            length = 'z';
            specifier = 'u';
        }
        switch (specifier) {
            case 'X':
                base = 16;
            case 'x':
                base = base == 10 ? 17 : base;
                if (altForm) {
                    if (print("0", 1)) ++chars;
                    // TODO: Add smt if the print fails
                }
            case 'u': {
                switch (length) {
                    case 0: {
                        unsigned int integer = va_arg(list, unsigned int);
                        int str_length = 0;
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec,
                                  leftJustify, zeroPad, &str_length);
                        if (print(intStrBuffer, str_length)) {
                            chars += str_length;
                        }
                        // TODO: Add smt if the print fails
                        break;
                    }
                    case 'H': {
                        int str_length = 0;
                        unsigned char integer = (unsigned char)va_arg(list, unsigned int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec,
                                  leftJustify, zeroPad, &str_length);
                        if (print(intStrBuffer, str_length)) {
                            chars += str_length;
                        }
                        // TODO: Add smt if the print fails
                        break;
                    }
                }
            }
        }
    }
}

int printf(const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%') amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, amount)) return -1;
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char)va_arg(parameters, int /* char promotes to int */);
            if (!maxrem) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(&c, sizeof(c))) return -1;
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(str, len)) return -1;
            written += len;
        } else if (*format == 'd') {
            format++;
            const double num = va_arg(parameters, const double);
        } else {
            // TODO: add support for other flags, and only print unknown until space
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, len)) return -1;
            written += len;
            format += len;
        }
    }

    va_end(parameters);
    return written;
}
