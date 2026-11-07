#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "food.h"

/* Width and height must be positive; body_segments must be in [0, width).
 * Returns NULL for invalid arguments or allocation failure. */
GridCell *snake_create(int body_segments, int width, int height);
void snake_destroy(GridCell *head);

/* Check the destination against the current body, including the tail. */
bool snake_would_collide(const GridCell *head, int dx, int dy, int width, int height);

typedef enum SnakeMoveResult {
    SNAKE_MOVE_ERROR = -1,
    SNAKE_MOVED,
    SNAKE_ATE
} SnakeMoveResult;

SnakeMoveResult snake_move(Food *food, GridCell **head, int dx, int dy, int width, int height);
void snake_draw(const GridCell *head, const Canvas *canvas, int cell_size, bool flash);

#endif
