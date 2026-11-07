#include "snake.h"
#include <stddef.h>
#include <stdlib.h>

void snake_destroy(Snake *snake) {
    GridCell *head = snake->head;
    while (head != NULL) {
        GridCell *next = head->next;
        free(head);
        head = next;
    }
    snake->head = NULL;
}

bool snake_create(Snake *snake, int body_segments, int width, int height) {
    *snake = (Snake){ .dx = 1, .next_dx = 1 };
    if (width <= 0 || height <= 0 || body_segments < 0 || body_segments >= width) return false;

    for (int i = body_segments; i >= 0; --i) {
        GridCell *segment = malloc(sizeof *segment);
        if (segment == NULL) {
            snake_destroy(snake);
            return false;
        }
        *segment = (GridCell){
            .x = (width / 2 - i % width + width) % width,
            .y = height / 2,
            .next = snake->head
        };
        snake->head = segment;
    }
    return true;
}

bool snake_would_collide(const Snake *snake, int width, int height) {
    const GridCell *head = snake->head;
    int dx = snake->dx, dy = snake->dy;
    int x = (head->x + dx + width) % width;
    int y = (head->y + dy + height) % height;
    for (const GridCell *cell = head->next; cell != NULL; cell = cell->next) {
        if (cell->x == x && cell->y == y) return true;
    }
    return false;
}

SnakeMoveResult snake_move(Snake *snake, Food *food, int width, int height) {
    GridCell **head = &snake->head;
    int dx = snake->dx, dy = snake->dy;
    int x = ((*head)->x + dx + width) % width;
    int y = ((*head)->y + dy + height) % height;
    if (food->active && food->x == x && food->y == y) {
        /* The new head is inserted in O(1); the old head becomes the first body segment. */
        GridCell *new_head = malloc(sizeof *new_head);
        if (new_head == NULL) return SNAKE_MOVE_ERROR;
        *new_head = (GridCell){ .x = x, .y = y, .next = *head };
        *head = new_head;
        food_spawn(food, *head, width, height);
        return SNAKE_ATE;
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
    return SNAKE_MOVED;
}

void snake_draw(const Snake *snake, const Canvas *canvas, int cell_size, bool flash) {
    for (const GridCell *segment = snake->head; segment != NULL; segment = segment->next) {
        draw_rectangle(canvas, segment->x * cell_size, segment->y * cell_size,
                       cell_size, cell_size, flash ? COLOR_COLLISION : 0xffd166);
    }
}
