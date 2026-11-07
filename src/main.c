#include <stdint.h>
#include <stdio.h>
#include "fenster.h"

enum { WIDTH = 160, HEIGHT = 120, SCALE = 4, FRAME_MS = 16 };
static uint32_t pixels[WIDTH * SCALE * HEIGHT * SCALE];

static void rectangle(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; ++py) {
        for (int px = x; px < x + w; ++px) {
            if (px < 0 || px >= WIDTH || py < 0 || py >= HEIGHT) continue;
            for (int sy = 0; sy < SCALE; ++sy)
                for (int sx = 0; sx < SCALE; ++sx)
                    pixels[(py * SCALE + sy) * (WIDTH * SCALE) + px * SCALE + sx] = color;
        }
    }
}

int main(void) {
    struct fenster window = {
        .title = "Pixel Game", .width = WIDTH * SCALE,
        .height = HEIGHT * SCALE, .buf = pixels
    };
    if (fenster_open(&window) != 0) {
        fputs("Could not open the game window.\n", stderr);
        return 1;
    }

    int x = WIDTH / 2 - 4, y = HEIGHT / 2 - 4;
    while (fenster_loop(&window) == 0 && !window.keys[27]) {
        int64_t start = fenster_time();
        x += !!window.keys['D'] + !!window.keys[19] - !!window.keys['A'] - !!window.keys[20];
        y += !!window.keys['S'] + !!window.keys[18] - !!window.keys['W'] - !!window.keys[17];
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > WIDTH - 8) x = WIDTH - 8;
        if (y > HEIGHT - 8) y = HEIGHT - 8;

        rectangle(0, 0, WIDTH, HEIGHT, 0x19243a);
        rectangle(x, y, 8, 8, 0xffd166);
        rectangle(x + 2, y + 2, 1, 1, 0x19243a);
        rectangle(x + 5, y + 2, 1, 1, 0x19243a);

        int64_t elapsed = fenster_time() - start;
        if (elapsed >= 0 && elapsed < FRAME_MS) fenster_sleep(FRAME_MS - elapsed);
    }
    fenster_close(&window);
    return 0;
}
