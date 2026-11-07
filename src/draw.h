#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>

enum { COLOR_COLLISION = 0xa52332 };

typedef struct Canvas {
    uint32_t *pixels;
    int width, height, scale;
} Canvas;

void draw_rectangle(const Canvas *canvas, int x, int y, int width, int height, uint32_t color);

#endif
