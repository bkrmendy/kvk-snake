#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include "food.h"

enum {
    WIDTH = 160, HEIGHT = 120, SCALE = 4,
    CELL_SIZE = 4, GRID_WIDTH = WIDTH / CELL_SIZE, GRID_HEIGHT = HEIGHT / CELL_SIZE - 2,
    BODY_SEGMENTS = 9
};

typedef struct Game {
    GridCell *head;
    Food food;
    unsigned int score;
    int move_ms;
    int dx, dy, next_dx, next_dy;
    int64_t last_move, flash_until, collision_time;
    bool game_over;
    bool paused, pause_key_down;
    int64_t paused_at;
} Game;

typedef enum GameResult {
    GAME_ERROR = -1,
    GAME_RUNNING
} GameResult;

bool game_init(Game *game, int64_t now);
bool game_restart(Game *game, int64_t now);
unsigned int game_food_score(const Game *game);
GameResult game_update(Game *game, const int keys[256], int64_t now);
void game_draw(const Game *game, const Canvas *canvas, int64_t now);
void game_destroy(Game *game);

#endif
