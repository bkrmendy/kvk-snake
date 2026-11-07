#include "game.h"
#include "snake.h"
#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum { MOVE_MS = 60, FLASH_MS = 60, COLLISION_FLASH_MS = 180, COLLISION_FLASHES = 3 };

bool game_init(Game *game, int64_t now) {
    *game = (Game){ .dx = 1, .next_dx = 1, .last_move = now };
    game->head = snake_create(BODY_SEGMENTS, GRID_WIDTH, GRID_HEIGHT);
    if (game->head == NULL) return false;
    srand((unsigned)time(NULL));
    food_spawn(&game->food, game->head, GRID_WIDTH, GRID_HEIGHT);
    return true;
}

GameResult game_update(Game *game, const int keys[256], int64_t now) {
    if (keys['P'] && !game->pause_key_down) {
        game->paused = !game->paused;
        if (game->paused) {
            game->paused_at = now;
        } else {
            int64_t duration = now > game->paused_at ? now - game->paused_at : 0;
            game->last_move += duration;
            if (game->flash_until > game->paused_at) game->flash_until += duration;
            if (game->game_over) game->collision_time += duration;
        }
    }
    game->pause_key_down = !!keys['P'];
    if (game->paused) return GAME_RUNNING;

    if (game->game_over) {
        if (now - game->collision_time >= COLLISION_FLASHES * 2 * COLLISION_FLASH_MS)
            return GAME_FINISHED;
        return GAME_RUNNING;
    }

    /* Ignore direct reversals so the head cannot turn into its neck. */
    if (keys[17] && game->dy != 1) { game->next_dx = 0; game->next_dy = -1; }
    else if (keys[18] && game->dy != -1) { game->next_dx = 0; game->next_dy = 1; }
    else if (keys[19] && game->dx != -1) { game->next_dx = 1; game->next_dy = 0; }
    else if (keys[20] && game->dx != 1) { game->next_dx = -1; game->next_dy = 0; }

    if (now < game->last_move) game->last_move = now;
    if (now - game->last_move >= MOVE_MS) {
        game->dx = game->next_dx;
        game->dy = game->next_dy;
        if (snake_would_collide(game->head, game->dx, game->dy, GRID_WIDTH, GRID_HEIGHT)) {
            game->game_over = true;
            game->collision_time = now;
        } else {
            int ate = snake_move(&game->food, &game->head, game->dx, game->dy, GRID_WIDTH, GRID_HEIGHT);
            if (ate < 0) return GAME_ERROR;
            if (ate) {
                ++game->score;
                game->flash_until = now + FLASH_MS;
            }
        }
        game->last_move = now;
    }
    return GAME_RUNNING;
}

void game_draw(const Game *game, const Canvas *canvas, int64_t now) {
    if (game->paused) now = game->paused_at;
    int64_t collision_elapsed = now - game->collision_time;
    bool collision_red = game->game_over && (collision_elapsed / COLLISION_FLASH_MS) % 2 == 0;
    draw_rectangle(canvas, 0, 0, WIDTH, HEIGHT,
                   collision_red || (!game->game_over && now < game->flash_until) ? 0xa52332 : 0x19243a);
    if (!collision_red) {
        food_draw(&game->food, canvas, CELL_SIZE);
        snake_draw(game->head, canvas, CELL_SIZE);
        draw_rectangle(canvas, 0, GRID_HEIGHT * CELL_SIZE, WIDTH,
                       HEIGHT - GRID_HEIGHT * CELL_SIZE, 0x3d5675);
        char score_label[32];
        snprintf(score_label, sizeof score_label, "SCORE: %u", game->score);
        font_draw_text(canvas, FONT_ADVANCE, GRID_HEIGHT * CELL_SIZE + 1,
                       score_label, 0xffffff);
        const char *controls = game->paused ? "P: RESUME Q: QUIT" : "P: PAUSE Q: QUIT";
        int length = 0;
        while (controls[length] != '\0') ++length;
        font_draw_text(canvas, WIDTH - FONT_ADVANCE - length * FONT_ADVANCE,
                       GRID_HEIGHT * CELL_SIZE + 1, controls, 0xffffff);
    }
}

void game_destroy(Game *game) {
    snake_destroy(game->head);
    game->head = NULL;
}
