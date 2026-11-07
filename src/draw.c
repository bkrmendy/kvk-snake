#include "draw.h"

void draw_rectangle(const Canvas *canvas, int x, int y, int width, int height, uint32_t color) {
    for (int py = y; py < y + height; ++py) {
        for (int px = x; px < x + width; ++px) {
            if (px < 0 || px >= canvas->width || py < 0 || py >= canvas->height) continue;
            for (int sy = 0; sy < canvas->scale; ++sy)
                for (int sx = 0; sx < canvas->scale; ++sx)
                    canvas->pixels[(py * canvas->scale + sy) * (canvas->width * canvas->scale)
                                   + px * canvas->scale + sx] = color;
        }
    }
}
