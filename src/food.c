#include "food.h"
#include <stdlib.h>

static bool occupied(const GridCell *snake, int x, int y) {
    for (const GridCell *segment = snake; segment != NULL; segment = segment->next) {
        if (segment->x == x && segment->y == y) return true;
    }
    return false;
}

void food_spawn(Food *food, const GridCell *snake, int width, int height) {
    /* Reservoir sampling selects a free cell without retrying on a full board. */
    int free_cells = 0;
    food->active = false;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!occupied(snake, x, y)) {
                ++free_cells;
                if (rand() % free_cells == 0) {
                    food->x = x;
                    food->y = y;
                    food->active = true;
                }
            }
        }
    }
}

void food_draw(const Food *food, const Canvas *canvas, int cell_size) {
    if (food->active) {
        draw_rectangle(canvas, food->x * cell_size, food->y * cell_size,
                       cell_size, cell_size, 0x65d46e);
    }
}
