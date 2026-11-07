#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include "grid_cell.h"

/* Check the destination against the current body, including the tail. */
bool snake_would_collide(const GridCell *head, int dx, int dy, int width, int height);

#endif
