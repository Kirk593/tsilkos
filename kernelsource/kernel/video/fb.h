#ifndef FB_H
#define FB_H

void fb_init(unsigned int *addr, int width, int height);
void draw_pixel(int x, int y, unsigned int color);
void draw_char(char c, int x, int y, unsigned int color);
void draw_string(char *str, int x, int y, unsigned int color);

#endif