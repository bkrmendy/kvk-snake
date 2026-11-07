#include "game.h"
#include "snake.h"
#include "font.h"
#include <stdio.h>

enum { INITIAL_MOVE_MS = 70, MIN_MOVE_MS = 30, FLASH_MS = 60,
       COLLISION_FLASH_MS = 180, COLLISION_FLASHES = 3 };

static const char HIGHSCORE_FILE[] = "snake_highscores.txt";

/* Fenster maps the arrow keys to these indexes. */
enum { KEY_UP = 17, KEY_DOWN = 18, KEY_RIGHT = 19, KEY_LEFT = 20 };

bool game_init(Game *game, int64_t now) {
    *game = (Game){ .last_move = now, .move_ms = INITIAL_MOVE_MS };
    if (!snake_create(&game->snake, BODY_SEGMENTS, GRID_WIDTH, GRID_HEIGHT)) return false;
    food_spawn(&game->food, game->snake.head, GRID_WIDTH, GRID_HEIGHT);
    if (!game->food.active) {
        game->game_over = true;
        game->collision_time = now;
    }
    return true;
}

bool game_restart(Game *game, int64_t now) {
    Game restarted;
    if (!game_init(&restarted, now)) return false;
    game_destroy(game);
    *game = restarted;
    return true;
}

static unsigned int game_food_score(const Game *game) {
    return (unsigned int)(INITIAL_MOVE_MS - game->move_ms);
}

static void highscore_system(Game *game) {
    if (!game->game_over || game->score_recorded) return;
    game->score_recorded = true;

    FILE *file = fopen(HIGHSCORE_FILE, "a");
    if (!file) {
        fprintf(stderr, "Could not write %s.\n", HIGHSCORE_FILE);
        return;
    }
    bool failed = fprintf(file, "%u\n", game->score) < 0;
    if (fclose(file) != 0) failed = true;
    if (failed) fprintf(stderr, "Could not write %s.\n", HIGHSCORE_FILE);
}

typedef enum StateResult { STATE_ERROR, STATE_STOP, STATE_CONTINUE } StateResult;

static StateResult game_state_system(Game *game, const int keys[256], int64_t now) {
    if (game->game_over) {
        if (keys['R']) {
            if (!game_restart(game, now)) return STATE_ERROR;
            game->pause_key_down = !!keys['P'];
        }
        return STATE_STOP;
    }
    if (keys['P'] && !game->pause_key_down) {
        game->paused = !game->paused;
        if (game->paused) {
            game->paused_at = now;
        } else {
            int64_t duration = now > game->paused_at ? now - game->paused_at : 0;
            game->last_move += duration;
            if (game->flash_until > game->paused_at) game->flash_until += duration;
        }
    }
    game->pause_key_down = !!keys['P'];
    return game->paused ? STATE_STOP : STATE_CONTINUE;
}

static void snake_input_system(Snake *snake, const int keys[256]) {
    /* Ignore direct reversals so the head cannot turn into its neck. */
    if (keys[KEY_UP] && snake->dy != 1) { snake->next_dx = 0; snake->next_dy = -1; }
    else if (keys[KEY_DOWN] && snake->dy != -1) { snake->next_dx = 0; snake->next_dy = 1; }
    else if (keys[KEY_RIGHT] && snake->dx != -1) { snake->next_dx = 1; snake->next_dy = 0; }
    else if (keys[KEY_LEFT] && snake->dx != 1) { snake->next_dx = -1; snake->next_dy = 0; }
}

static GameResult snake_movement_system(Game *game, int64_t now) {
    if (now < game->last_move) game->last_move = now;
    if (now - game->last_move >= game->move_ms) {
        game->snake.dx = game->snake.next_dx;
        game->snake.dy = game->snake.next_dy;
        if (snake_would_collide(&game->snake, GRID_WIDTH, GRID_HEIGHT)) {
            game->game_over = true;
            game->collision_time = now;
        } else {
            SnakeMoveResult move = snake_move(&game->snake, &game->food, GRID_WIDTH, GRID_HEIGHT);
            if (move == SNAKE_MOVE_ERROR) return GAME_ERROR;
            if (move == SNAKE_ATE) {
                if (game->move_ms > MIN_MOVE_MS) --game->move_ms;
                game->score += game_food_score(game);
                game->flash_until = now + FLASH_MS;
                if (!game->food.active) {
                    game->game_over = true;
                    game->collision_time = now;
                }
            }
        }
        game->last_move = now;
    }
    return GAME_RUNNING;
}

GameResult game_update(Game *game, const int keys[256], int64_t now) {
    highscore_system(game);
    StateResult state = game_state_system(game, keys, now);
    if (state == STATE_ERROR) return GAME_ERROR;
    if (state == STATE_STOP) return GAME_RUNNING;
    snake_input_system(&game->snake, keys);
    GameResult result = snake_movement_system(game, now);
    highscore_system(game);
    return result;
}

void game_draw(const Game *game, const Canvas *canvas, int64_t now) {
    if (game->paused) now = game->paused_at;
    int64_t collision_elapsed = now - game->collision_time;
    bool collision_red = game->game_over && collision_elapsed >= 0 &&
                         collision_elapsed < COLLISION_FLASHES * 2 * COLLISION_FLASH_MS &&
                         (collision_elapsed / COLLISION_FLASH_MS) % 2 == 0;
    draw_rectangle(canvas, 0, 0, WIDTH, HEIGHT, collision_red ? COLOR_COLLISION : 0x19243a);
    if (!collision_red) {
        food_draw(&game->food, canvas, CELL_SIZE);
        snake_draw(&game->snake, canvas, CELL_SIZE, !game->game_over && now < game->flash_until);
        draw_rectangle(canvas, 0, GRID_HEIGHT * CELL_SIZE, WIDTH,
                       HEIGHT - GRID_HEIGHT * CELL_SIZE, 0x3d5675);
        char score_label[32];
        snprintf(score_label, sizeof score_label, "SCORE: %u", game->score);
        font_draw_text(canvas, FONT_ADVANCE, GRID_HEIGHT * CELL_SIZE + 1,
                       score_label, 0xffffff);
        const char *controls = game->game_over ? "R: RESTART Q: QUIT" :
                               game->paused ? "P: RESUME Q: QUIT" : "P: PAUSE Q: QUIT";
        int length = 0;
        while (controls[length] != '\0') ++length;
        font_draw_text(canvas, WIDTH - FONT_ADVANCE - length * FONT_ADVANCE,
                       GRID_HEIGHT * CELL_SIZE + 1, controls, 0xffffff);
    }
}

void game_destroy(Game *game) {
    snake_destroy(&game->snake);
}
