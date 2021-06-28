#ifndef SCREEN__H
#define SCREEN__H
#include <stdint.h>
#include "const.h"


class Screen
/* Works as a fake screen for tests */
{
    public:
    void init();

    void clear();
    void clear(uint8_t c);
    void clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t c = 0);

    void refresh();
    void invertDisplay(bool invert);
    void setCursor(uint8_t x, uint8_t y);
    void write(uint8_t x, uint8_t y, uint8_t b);
    void scroll(int8_t n);

    // Yes, this wastes memory, but I don't want to
    // have to use things like "cursor_x-1" all the time.
    uint8_t _screen[SCREEN_COLS + 1][SCREEN_ROWS + 1];

};

#endif // SCREEN__H