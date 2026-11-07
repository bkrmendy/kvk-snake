#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "fenster.h"

enum { FRAME_MS = 16 };
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
    Game game;
    srand((unsigned)time(NULL));
    if (!game_init(&game, fenster_time())) {
        fputs("Could not allocate snake segment.\n", stderr);
        fenster_close(&window);
        return 1;
    }

    int exit_code = 0;
    while (fenster_loop(&window) == 0 && !window.keys[27] && !window.keys['Q']) {
        int64_t start = fenster_time();
        GameResult result = game_update(&game, window.keys, start);
        if (result != GAME_RUNNING) {
            if (result == GAME_ERROR) {
                fputs("Could not grow snake.\n", stderr);
                exit_code = 1;
            }
            break;
        }
        game_draw(&game, &canvas, start);
        int64_t elapsed = fenster_time() - start;
        if (elapsed >= 0 && elapsed < FRAME_MS) fenster_sleep(FRAME_MS - elapsed);
    }
    game_destroy(&game);
    fenster_close(&window);
    return exit_code;
}
