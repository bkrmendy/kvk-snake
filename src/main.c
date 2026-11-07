#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "food.h"
#include "fenster.h"

enum {
    WIDTH = 160, HEIGHT = 120, SCALE = 4, FRAME_MS = 16,
    CELL_SIZE = 4, GRID_WIDTH = WIDTH / CELL_SIZE, GRID_HEIGHT = HEIGHT / CELL_SIZE,
    BODY_SEGMENTS = 9, MOVE_MS = 60, FLASH_MS = 60
};
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

    GridCell *head = NULL;
    /* Build from tail to head so each node can later be freed independently. */
    for (int i = BODY_SEGMENTS; i >= 0; --i) {
        GridCell *segment = malloc(sizeof *segment);
        if (segment == NULL) {
            fputs("Could not allocate snake segment.\n", stderr);
            while (head != NULL) {
                GridCell *next = head->next;
                free(head);
                head = next;
            }
            fenster_close(&window);
            return 1;
        }
        *segment = (GridCell){
            .x = (GRID_WIDTH / 2 - i + GRID_WIDTH) % GRID_WIDTH,
            .y = GRID_HEIGHT / 2,
            .next = head
        };
        head = segment;
    }
    srand((unsigned)time(NULL));
    Food food = {0};
    food_spawn(&food, head, GRID_WIDTH, GRID_HEIGHT);
    int64_t flash_until = 0;
    int exit_code = 0;
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
            int ate = food_move_snake(&food, &head, dx, dy, GRID_WIDTH, GRID_HEIGHT);
            if (ate < 0) {
                fputs("Could not grow snake.\n", stderr);
                exit_code = 1;
                break;
            }
            if (ate) flash_until = start + FLASH_MS;
            last_move = start;
        }

        rectangle(0, 0, WIDTH, HEIGHT, start < flash_until ? 0xa52332 : 0x19243a);
        if (food.active) {
            rectangle(food.x * CELL_SIZE, food.y * CELL_SIZE,
                      CELL_SIZE, CELL_SIZE, 0x65d46e);
        }
        for (GridCell *segment = head; segment != NULL; segment = segment->next) {
            rectangle(segment->x * CELL_SIZE, segment->y * CELL_SIZE,
                      CELL_SIZE, CELL_SIZE, 0xffd166);
        }

        int64_t elapsed = fenster_time() - start;
        if (elapsed >= 0 && elapsed < FRAME_MS) fenster_sleep(FRAME_MS - elapsed);
    }
    while (head != NULL) {
        GridCell *next = head->next;
        free(head);
        head = next;
    }
    fenster_close(&window);
    return exit_code;
}
