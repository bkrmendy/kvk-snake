#include <stdint.h>
#include <stdio.h>
#include "fenster.h"

enum {
    WIDTH = 160, HEIGHT = 120, SCALE = 4, FRAME_MS = 16,
    CELL_SIZE = 4, GRID_WIDTH = WIDTH / CELL_SIZE, GRID_HEIGHT = HEIGHT / CELL_SIZE,
    BODY_SEGMENTS = 9, MOVE_MS = 60
};
static uint32_t pixels[WIDTH * SCALE * HEIGHT * SCALE];

typedef struct Segment {
    int x, y; /* Grid coordinates. */
    struct Segment *next;
} Segment;

/* Move each segment into the previous position of the segment ahead of it. */
static void move_snake(Segment *head, int dx, int dy) {
    int previous_x = head->x, previous_y = head->y;
    head->x = (head->x + dx + GRID_WIDTH) % GRID_WIDTH;
    head->y = (head->y + dy + GRID_HEIGHT) % GRID_HEIGHT;
    for (Segment *segment = head->next; segment != NULL; segment = segment->next) {
        int x = segment->x, y = segment->y;
        segment->x = previous_x;
        segment->y = previous_y;
        previous_x = x;
        previous_y = y;
    }
}

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

    Segment segments[BODY_SEGMENTS + 1];
    for (int i = 0; i <= BODY_SEGMENTS; ++i) {
        segments[i] = (Segment){
            .x = (GRID_WIDTH / 2 - i + GRID_WIDTH) % GRID_WIDTH,
            .y = GRID_HEIGHT / 2,
            .next = i < BODY_SEGMENTS ? &segments[i + 1] : NULL
        };
    }
    int dx = 1, dy = 0, next_dx = dx, next_dy = dy;
    int64_t last_move = fenster_time();
    while (fenster_loop(&window) == 0 && !window.keys[27]) {
        int64_t start = fenster_time();
        /* Ignore direct reversals so the head cannot turn into its neck. */
        if (window.keys[17] && dy != 1) { next_dx = 0; next_dy = -1; }
        else if (window.keys[18] && dy != -1) { next_dx = 0; next_dy = 1; }
        else if (window.keys[19] && dx != -1) { next_dx = 1; next_dy = 0; }
        else if (window.keys[20] && dx != 1) { next_dx = -1; next_dy = 0; }

        if (start < last_move) last_move = start;
        if (start - last_move >= MOVE_MS) {
            dx = next_dx;
            dy = next_dy;
            move_snake(&segments[0], dx, dy);
            last_move = start;
        }

        rectangle(0, 0, WIDTH, HEIGHT, 0x19243a);
        for (Segment *segment = &segments[0]; segment != NULL; segment = segment->next) {
            rectangle(segment->x * CELL_SIZE, segment->y * CELL_SIZE,
                      CELL_SIZE, CELL_SIZE, 0xffd166);
        }

        int64_t elapsed = fenster_time() - start;
        if (elapsed >= 0 && elapsed < FRAME_MS) fenster_sleep(FRAME_MS - elapsed);
    }
    fenster_close(&window);
    return 0;
}
