#ifndef FB_H
#define FB_H

#include <stdint.h>

void fb_init(
    void *addr,
    uint32_t width,
    uint32_t height,
    uint32_t pitch
);

void draw_pixel(
    uint32_t x,
    uint32_t y,
    uint32_t color
);

void draw_string(
    const char *str,
    uint32_t x,
    uint32_t y,
    uint32_t color
);

#endif