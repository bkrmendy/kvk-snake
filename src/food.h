#ifndef FOOD_H
#define FOOD_H

#include <stdbool.h>
#include "grid_cell.h"
#include "draw.h"

typedef struct Food {
    int x, y;
    bool active;
} Food;

/* Pick an unoccupied cell; inactive means the grid is full. */
void food_spawn(Food *food, const GridCell *snake, int width, int height);

void food_draw(const Food *food, const Canvas *canvas, int cell_size);

#endif
