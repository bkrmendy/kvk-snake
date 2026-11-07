#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "food.h"

typedef struct Snake {
    GridCell *head;
    int dx, dy, next_dx, next_dy;
} Snake;

/* Width and height must be positive; body_segments must be in [0, width).
 * Returns false for invalid arguments or allocation failure. */
bool snake_create(Snake *snake, int body_segments, int width, int height);
void snake_destroy(Snake *snake);

/* Check the destination against the current body, including the tail. */
bool snake_would_collide(const Snake *snake, int width, int height);

typedef enum SnakeMoveResult {
    SNAKE_MOVE_ERROR = -1,
    SNAKE_MOVED,
    SNAKE_ATE
} SnakeMoveResult;

SnakeMoveResult snake_move(Snake *snake, Food *food, int width, int height);
void snake_draw(const Snake *snake, const Canvas *canvas, int cell_size, bool flash);

#endif
