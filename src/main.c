#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "fenster.h"

enum { FRAME_MS = 16 };

typedef struct App {
    struct fenster window;
    Canvas canvas;
    Game game;
    uint32_t pixels[WIDTH * SCALE * HEIGHT * SCALE];
} App;

static App app = {
    .window = { .title = "Pixel Game", .width = WIDTH * SCALE, .height = HEIGHT * SCALE }
};

static void time_init(void) {
    srand((unsigned)time(NULL));
}

static bool app_init(App *app) {
    app->window.buf = app->pixels;
    if (fenster_open(&app->window) != 0) {
        fputs("Could not open the game window.\n", stderr);
        return false;
    }

    app->canvas = (Canvas){ .pixels = app->pixels, .width = WIDTH, .height = HEIGHT, .scale = SCALE };
    time_init();
    if (!game_init(&app->game, fenster_time())) {
        fputs("Could not initialize snake (invalid configuration or allocation failure).\n", stderr);
        fenster_close(&app->window);
        return false;
    }
    return true;
}

static int app_run(App *app) {
    while (fenster_loop(&app->window) == 0 && !app->window.keys[27] && !app->window.keys['Q']) {
        int64_t start = fenster_time();
        if (game_update(&app->game, app->window.keys, start) == GAME_ERROR) {
            fputs("Could not grow snake.\n", stderr);
            return 1;
        }
        game_draw(&app->game, &app->canvas, start);
        int64_t elapsed = fenster_time() - start;
        if (elapsed >= 0 && elapsed < FRAME_MS) fenster_sleep(FRAME_MS - elapsed);
    }
    return 0;
}

static void app_destroy(App *app) {
    game_destroy(&app->game);
    fenster_close(&app->window);
}

int main(void) {
    if (!app_init(&app)) return 1;
    int result = app_run(&app);
    app_destroy(&app);
    return result;
}
