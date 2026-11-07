#ifndef FOOD_H
#define FOOD_H

#include <stdbool.h>
#include "grid_cell.h"

typedef struct Food {
    int x, y;
    bool active;
} Food;

/* Pick an unoccupied cell; inactive means the grid is full. */
void food_spawn(Food *food, const GridCell *snake, int width, int height);

/* Move the snake and return 1 if it ate, 0 otherwise, or -1 on allocation failure. */
int food_move_snake(Food *food, GridCell **head, int dx, int dy, int width, int height);

#endif
