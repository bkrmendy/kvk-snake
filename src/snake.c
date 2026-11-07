#include "snake.h"
#include <stddef.h>
#include <stdlib.h>

void snake_destroy(GridCell *head) {
    while (head != NULL) {
        GridCell *next = head->next;
        free(head);
        head = next;
    }
}

GridCell *snake_create(int body_segments, int width, int height) {
    GridCell *head = NULL;
    for (int i = body_segments; i >= 0; --i) {
        GridCell *segment = malloc(sizeof *segment);
        if (segment == NULL) {
            snake_destroy(head);
            return NULL;
        }
        *segment = (GridCell){
            .x = (width / 2 - i % width + width) % width,
            .y = height / 2,
            .next = head
        };
        head = segment;
    }
    return head;
}

bool snake_would_collide(const GridCell *head, int dx, int dy, int width, int height) {
    int x = (head->x + dx + width) % width;
    int y = (head->y + dy + height) % height;
    for (const GridCell *cell = head->next; cell != NULL; cell = cell->next) {
        if (cell->x == x && cell->y == y) return true;
    }
    return false;
}

int snake_move(Food *food, GridCell **head, int dx, int dy, int width, int height) {
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

void snake_draw(const GridCell *head, const Canvas *canvas, int cell_size) {
    for (const GridCell *segment = head; segment != NULL; segment = segment->next) {
        draw_rectangle(canvas, segment->x * cell_size, segment->y * cell_size,
                       cell_size, cell_size, 0xffd166);
    }
}
