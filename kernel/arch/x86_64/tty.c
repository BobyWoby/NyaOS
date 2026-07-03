#include <kernel/limine.h>
#include <kernel/requests.h>
#include <kernel/system.h>
#include <kernel/tty.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static size_t terminal_row;
static size_t terminal_column;
static color_t terminal_fg = {202, 211, 245};
static color_t terminal_bg = {48, 52, 70};
static uint16_t* terminal_buffer;
static struct limine_file* font_file;
static PSF_font* font_header;
static struct limine_framebuffer* fb;
uint32_t font_size = 2;



void putpixel(color_t c, unsigned int x, unsigned int y) {
    volatile uint32_t* fb_ptr = fb->address;
    uint32_t idx = y * (fb->pitch / 4) + x;
    fb_ptr[idx] = (c.r << 16) | (c.g << 8) | (c.b);
}

void fillrect(color_t c, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    uint32_t color = (c.r << 16) | (c.g << 8) | (c.b);
    volatile uint32_t* fb_ptr = fb->address;
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            fb_ptr[j] = color;
        }
        fb_ptr += fb->pitch / 4;
    }
}

void psf_init() { font_header = (PSF_font*)font_file->address; }

void draw_char(unsigned char c, int x, int y, color_t color, uint32_t pixel_size) {
    unsigned char* glyph =
        (unsigned char*)font_file->address + font_header->headersize +
        (c > 0 && c < font_header->numglyph ? c : 0) * font_header->bytesperglyph;

    uint32_t bytesPerGlyphLine = (font_header->width + 7) / 8;
    for (int i = 0; i < font_header->height; ++i) {
        unsigned char* currentByte = glyph + (bytesPerGlyphLine * i);
        uint8_t mask = 1 << 7;
        for (int j = 0; j < font_header->width; ++j) {
            if (*currentByte & mask) {
                for (int px = 0; px < pixel_size; ++px) {
                    for (int py = 0; py < pixel_size; ++py) {
                        putpixel(color, x + (j * pixel_size) + px, y + (i * pixel_size) + py);
                    }
                }
            }
            mask >>= 1;
            if (mask == 0) {
                mask = 1 << 7;
                currentByte += 1;
            }
        }
    }
}

void terminal_putchar(char c);
void terminal_initialize(void) {
    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL ||
        framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    fb = framebuffer_request.response->framebuffers[0];
    font_file = module_request.response->modules[0];

    psf_init();

    volatile uint32_t* fb_ptr = fb->address;
    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {
            // uint32_t nX = x * 255 / fb->width;
            // uint32_t nY = y * 255 / fb->height;
            // color_t c = {0, nX, nY};
            putpixel(terminal_bg, x, y);
            // fb_ptr[y * (fb->pitch / 4) + x] = (nY << 8) | nX;
        }
    }
    // draw_char('h', 0, 0, terminal_fg, 4);
    // terminal_putchar('h');
    // color_t c = {255, 255, 255};
    // fillrect(c , 0, 0, 200, 200);
}

void terminal_putchar(char c) {
    unsigned char uc = c;
    // TODO: parse other escape characters?
    if (uc == '\n') {
        terminal_column = 0;
        if (++terminal_row == fb->height / (font_header->height * font_size)){
            clear_screen();
            terminal_row = 0;
        } 
        return;
    }

    draw_char(uc, terminal_column * font_header->width * font_size,
              terminal_row * font_header->height * font_size, terminal_fg, font_size);
    // terminal_putentryat(uc, terminal_column, terminal_row);

    if (++terminal_column == fb->width / (font_header->width * font_size)) {
        terminal_column = 0;
        if (++terminal_row == fb->height / (font_header->height * font_size)){
            clear_screen();
            terminal_row = 0;
        } 
    }
}

void clear_screen(){
    volatile uint32_t* fb_ptr = fb->address;
    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {
            // uint32_t nX = x * 255 / fb->width;
            // uint32_t nY = y * 255 / fb->height;
            // color_t c = {0, nX, nY};
            putpixel(terminal_bg, x, y);
            // fb_ptr[y * (fb->pitch / 4) + x] = (nY << 8) | nX;
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) { terminal_write(data, strlen(data)); }
