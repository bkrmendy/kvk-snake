#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "food.h"

/* Create a horizontal snake; returns NULL if allocation fails. */
GridCell *snake_create(int body_segments, int width, int height);
void snake_destroy(GridCell *head);

/* Check the destination against the current body, including the tail. */
bool snake_would_collide(const GridCell *head, int dx, int dy, int width, int height);

/* Return 1 if food was eaten, 0 for a normal move, or -1 on allocation failure. */
int snake_move(Food *food, GridCell **head, int dx, int dy, int width, int height);
void snake_draw(const GridCell *head, const Canvas *canvas, int cell_size);

#endif
