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

int food_move_snake(Food *food, GridCell **head, int dx, int dy, int width, int height) {
    int x = ((*head)->x + dx + width) % width;
    int y = ((*head)->y + dy + height) % height;
    if (food->active && food->x == x && food->y == y) {
        /* The new head is inserted in O(1); the old head becomes the first body segment. */
        GridCell *new_head = malloc(sizeof *new_head);
        if (new_head == NULL) return -1;
        *new_head = (GridCell){ .x = x, .y = y, .next = *head };
        *head = new_head;
        food_spawn(food, *head, width, height);
        return 1;
    }

    int previous_x = (*head)->x, previous_y = (*head)->y;
    (*head)->x = x;
    (*head)->y = y;
    for (GridCell *segment = (*head)->next; segment != NULL; segment = segment->next) {
        int old_x = segment->x, old_y = segment->y;
        segment->x = previous_x;
        segment->y = previous_y;
        previous_x = old_x;
        previous_y = old_y;
    }
    return 0;
}
