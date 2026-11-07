#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "food.h"
#include "snake.h"
#include "fenster.h"

enum {
    WIDTH = 160, HEIGHT = 120, SCALE = 4, FRAME_MS = 16,
    CELL_SIZE = 4, GRID_WIDTH = WIDTH / CELL_SIZE, GRID_HEIGHT = HEIGHT / CELL_SIZE,
    BODY_SEGMENTS = 9, MOVE_MS = 60, FLASH_MS = 60,
    COLLISION_FLASH_MS = 180, COLLISION_FLASHES = 3
};
static uint32_t pixels[WIDTH * SCALE * HEIGHT * SCALE];

int main(void) {
    struct fenster window = {
        .title = "Pixel Game", .width = WIDTH * SCALE,
        .height = HEIGHT * SCALE, .buf = pixels
    };
    if (fenster_open(&window) != 0) {
        fputs("Could not open the game window.\n", stderr);
        return 1;
    }

    Canvas canvas = { .pixels = pixels, .width = WIDTH, .height = HEIGHT, .scale = SCALE };
    GridCell *head = snake_create(BODY_SEGMENTS, GRID_WIDTH, GRID_HEIGHT);
    if (head == NULL) {
        fputs("Could not allocate snake segment.\n", stderr);
        fenster_close(&window);
        return 1;
    }
    srand((unsigned)time(NULL));
    Food food = {0};
    food_spawn(&food, head, GRID_WIDTH, GRID_HEIGHT);
    int64_t flash_until = 0;
    int exit_code = 0;
    bool game_over = false;
    int64_t collision_time = 0;
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
        if (!game_over && start - last_move >= MOVE_MS) {
            dx = next_dx;
            dy = next_dy;
            if (snake_would_collide(head, dx, dy, GRID_WIDTH, GRID_HEIGHT)) {
                game_over = true;
                collision_time = start;
            } else {
                int ate = snake_move(&food, &head, dx, dy, GRID_WIDTH, GRID_HEIGHT);
                if (ate < 0) {
                    fputs("Could not grow snake.\n", stderr);
                    exit_code = 1;
                    break;
                }
                if (ate) flash_until = start + FLASH_MS;
            }
            last_move = start;
        }

        int64_t collision_elapsed = start - collision_time;
        if (game_over && collision_elapsed >= COLLISION_FLASHES * 2 * COLLISION_FLASH_MS) {
            break;
        }
        bool collision_red = game_over && (collision_elapsed / COLLISION_FLASH_MS) % 2 == 0;
        draw_rectangle(&canvas, 0, 0, WIDTH, HEIGHT,
                  collision_red || (!game_over && start < flash_until) ? 0xa52332 : 0x19243a);
        if (!collision_red) {
            food_draw(&food, &canvas, CELL_SIZE);
            snake_draw(head, &canvas, CELL_SIZE);
        }

        int64_t elapsed = fenster_time() - start;
        if (elapsed >= 0 && elapsed < FRAME_MS) fenster_sleep(FRAME_MS - elapsed);
    }
    snake_destroy(head);
    fenster_close(&window);
    return exit_code;
}
