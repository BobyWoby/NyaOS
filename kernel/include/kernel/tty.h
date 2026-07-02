#pragma once
#include <stddef.h>
#include <stdint.h>
#define PSF_FONT_MAGIC 0x864ab572

typedef struct color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}color_t;


/*
    PSF2
*/
typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void clear_screen();
