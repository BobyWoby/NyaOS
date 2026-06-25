#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}color_t;

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

