#include "font.h"

static unsigned int *fb;
static int fb_width;
static int fb_height;

void draw_pixel(int x, int y, unsigned int color) {
    fb[y * fb_width + x] = color;
}

void fb_init(unsigned int *addr, int width, int height) {
    fb = addr;
    fb_width = width;
    fb_height = height;
}

void draw_char(char c, int x, int y, unsigned int color) {
    const unsigned char *glyph = font_8x8[(unsigned char)c];

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (glyph[row] & (1 << (7 - col))) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void draw_string(char *str, int x, int y, unsigned int color) {
    while (*str) {
        draw_char(*str, x, y, color);
        x += 8;
        str++;
    }
}
