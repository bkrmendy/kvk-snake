#ifndef GRID_CELL_H
#define GRID_CELL_H

typedef struct GridCell {
    int x, y; /* Grid coordinates. */
    struct GridCell *next;
} GridCell;

#endif
