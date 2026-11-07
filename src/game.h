#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include "food.h"

enum {
    WIDTH = 160, HEIGHT = 120, SCALE = 4,
    CELL_SIZE = 4, GRID_WIDTH = WIDTH / CELL_SIZE, GRID_HEIGHT = HEIGHT / CELL_SIZE,
    BODY_SEGMENTS = 9
};

typedef struct Game {
    GridCell *head;
    Food food;
    int dx, dy, next_dx, next_dy;
    int64_t last_move, flash_until, collision_time;
    bool game_over;
} Game;

typedef enum GameResult {
    GAME_ERROR = -1,
    GAME_RUNNING,
    GAME_FINISHED
} GameResult;

bool game_init(Game *game, int64_t now);
GameResult game_update(Game *game, const int keys[256], int64_t now);
void game_draw(const Game *game, const Canvas *canvas, int64_t now);
void game_destroy(Game *game);

#endif
