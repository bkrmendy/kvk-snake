#include "font.h"
#include <stddef.h>

/* Each row uses its low three bits, with the leftmost pixel in bit 2. */
static const uint8_t letters[26][FONT_HEIGHT] = {
    {2,5,7,5,5}, {6,5,6,5,6}, {3,4,4,4,3}, {6,5,5,5,6},
    {7,4,6,4,7}, {7,4,6,4,4}, {3,4,5,5,3}, {5,5,7,5,5},
    {7,2,2,2,7}, {1,1,1,5,2}, {5,5,6,5,5}, {4,4,4,4,7},
    {5,7,7,5,5}, {5,7,7,7,5}, {2,5,5,5,2}, {6,5,6,4,4},
    {2,5,5,3,1}, {6,5,6,5,5}, {3,4,2,1,6}, {7,2,2,2,2},
    {5,5,5,5,7}, {5,5,5,5,2}, {5,5,7,7,5}, {5,5,2,5,5},
    {5,5,2,2,2}, {7,1,2,4,7}
};
static const uint8_t digits[10][FONT_HEIGHT] = {
    {7,5,5,5,7}, {2,6,2,2,7}, {6,1,2,4,7}, {6,1,2,1,6},
    {5,5,7,1,1}, {7,4,6,1,6}, {3,4,7,5,7}, {7,1,2,2,2},
    {7,5,7,5,7}, {7,5,7,1,6}
};
static const uint8_t space[FONT_HEIGHT] = {0,0,0,0,0};
static const uint8_t dot[FONT_HEIGHT] = {0,0,0,0,2};
static const uint8_t colon[FONT_HEIGHT] = {0,2,0,2,0};
static const uint8_t dash[FONT_HEIGHT] = {0,0,7,0,0};
static const uint8_t exclamation[FONT_HEIGHT] = {2,2,2,0,2};
static const uint8_t question[FONT_HEIGHT] = {6,1,2,0,2};

static const uint8_t *glyph(unsigned char c) {
    if (c >= 'a' && c <= 'z') c = (unsigned char)(c - 'a' + 'A');
    if (c >= 'A' && c <= 'Z') return letters[c - 'A'];
    if (c >= '0' && c <= '9') return digits[c - '0'];
    switch (c) {
        case ' ': return space;
        case '.': return dot;
        case ':': return colon;
        case '-': return dash;
        case '!': return exclamation;
        default: return question;
    }
}

void font_draw_text(const Canvas *canvas, int x, int y, const char *text, uint32_t color) {
    int start_x = x;
    for (const unsigned char *c = (const unsigned char *)text; *c != '\0'; ++c) {
        if (*c == '\n') {
            x = start_x;
            y += FONT_HEIGHT + 1;
            continue;
        }
        const uint8_t *rows = glyph(*c);
        for (int row = 0; row < FONT_HEIGHT; ++row) {
            for (int col = 0; col < FONT_WIDTH; ++col) {
                if (rows[row] & (1u << (FONT_WIDTH - 1 - col)))
                    draw_rectangle(canvas, x + col, y + row, 1, 1, color);
            }
        }
        x += FONT_ADVANCE;
    }
}
