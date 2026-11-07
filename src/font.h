#ifndef FONT_H
#define FONT_H

#include "draw.h"

/* A 3x5 font with one pixel of spacing; lowercase is drawn as uppercase.
 * Supports letters, digits, space, '.', ':', '-', '!', and '?'. Other
 * characters are drawn as '?'. Coordinates are in logical canvas pixels. */
enum { FONT_WIDTH = 3, FONT_HEIGHT = 5, FONT_ADVANCE = 4 };
void font_draw_text(const Canvas *canvas, int x, int y, const char *text, uint32_t color);

#endif
