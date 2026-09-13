#include "fb.h"
#include "font.h"

static uint32_t *fb;
static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;

void fb_init(void *addr, uint32_t width, uint32_t height, uint32_t pitch)
{
    fb = (uint32_t *)addr;
    fb_width = width;
    fb_height = height;
    fb_pitch = pitch;
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    if (!fb || x >= fb_width || y >= fb_height || fb_pitch < 4)
        return;

    uint32_t pixels_per_row = fb_pitch / 4;
    fb[y * pixels_per_row + x] = color;
}

static void draw_char(char c, uint32_t x, uint32_t y, uint32_t color)
{
    unsigned char ch = (unsigned char)c;

    if (ch < 32 || ch > 122)
        ch = '?';

    const unsigned char *glyph = font_8x8[ch - 32];

    for (uint32_t row = 0; row < 8; row++) {
        for (uint32_t col = 0; col < 8; col++) {

            if (glyph[row] & (1u << col)) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void draw_string(const char *str, uint32_t x, uint32_t y, uint32_t color)
{
    if (!str)
        return;

    while (*str) {
        draw_char(*str++, x, y, color);
        x += 8;
    }
}
