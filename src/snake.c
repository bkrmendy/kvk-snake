#include "snake.h"
#include <stddef.h>

bool snake_would_collide(const GridCell *head, int dx, int dy, int width, int height) {
    int x = (head->x + dx + width) % width;
    int y = (head->y + dy + height) % height;
    for (const GridCell *cell = head->next; cell != NULL; cell = cell->next) {
        if (cell->x == x && cell->y == y) return true;
    }
    return false;
}
