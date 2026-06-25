#include <kernel/limine.h>
#include <kernel/system.h>
#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;
static struct limine_file* font_file;
static PSF_font *font_header;

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST_ID, .revision = 0};

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST_ID, .revision = 0};

void putpixel(struct limine_framebuffer* framebuffer, color_t c, unsigned int x, unsigned int y) {
    volatile uint32_t* fb_ptr = framebuffer->address;
    uint32_t idx = y * (framebuffer->pitch / 4) + x;
    fb_ptr[idx] = (c.r << 16) | (c.g << 8) | (c.b);
}

void fillrect(struct limine_framebuffer* framebuffer, color_t c, uint32_t x, uint32_t y, uint32_t w,
              uint32_t h) {
    uint32_t color = (c.r << 16) | (c.g << 8) | (c.b);
    volatile uint32_t* fb_ptr = framebuffer->address;
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            fb_ptr[j] = color;
        }
        fb_ptr += framebuffer->pitch / 4;
    }
}

void psf_init() { font_header = (PSF_font*)font_file->address; }

void draw_char(struct limine_framebuffer* fb,unsigned char c, int x, int y, color_t color) {
    unsigned char* glyph = (unsigned char*)font_file->address + font_header->headersize +
                           (c > 0 && c < font_header->numglyph ? c : 0) * font_header->bytesperglyph;

    uint32_t bytesPerGlyphLine = (font_header->width + 7) / 8;
    for (int i = 0; i < font_header->height; ++i) {
        unsigned char* currentByte = glyph + (bytesPerGlyphLine * i);
        uint8_t mask = 1 << 7;
        for (int j = 0; j < font_header->width; ++j) {
            if (*currentByte & mask) {
                putpixel(fb, color, x + j, y + i);
            }
            mask >>= 1;
            if (mask == 0) {
                mask = 1 << 7;
                currentByte += 1;
            }
        }
    }
}

void terminal_initialize(void) {
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];
    font_file = module_request.response->modules[0];

    psf_init();

    volatile uint32_t* fb_ptr = framebuffer->address;
    for (size_t y = 0; y < framebuffer->height; y++) {
        for (size_t x = 0; x < framebuffer->width; x++) {
            uint32_t nX = x * 255 / framebuffer->width;
            uint32_t nY = y * 255 / framebuffer->height;
            color_t c = {0, nX, nY};
            putpixel(framebuffer, c, x, y);
            // fb_ptr[y * (framebuffer->pitch / 4) + x] = (nY << 8) | nX;
        }
    }
    color_t c = {255, 255, 255};
    int cx = 0, cy = 0;
    draw_char(framebuffer, 'H', cx, cy, c);
    cx += 8;
    draw_char(framebuffer, 'e', cx, cy, c);
    cx += 8;
    draw_char(framebuffer, 'l', cx, cy, c);
    cx += 8;
    draw_char(framebuffer, 'l', cx, cy, c);
    cx += 8;
    draw_char(framebuffer, 'o', cx, cy, c);
    // fillrect(framebuffer, c , 0, 0, 200, 200);
}

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    unsigned char uc = c;
    // TODO: parse other escape characters?
    if (uc == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_row = 0;
    }

    terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_row = 0;
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) { terminal_write(data, strlen(data)); }
