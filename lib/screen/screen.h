#ifndef SCREEN__H
#define SCREEN__H
#include <stdint.h>
#include "const.h"

// Screen data. Yes, this wastes one row and one column.
extern uint8_t _screen[SCREEN_COLS + 1][SCREEN_ROWS + 1];

class Screen
/* Works as a fake screen for tests */
{
    public:
    virtual void init();

    virtual void clear();
    virtual void clear(uint8_t c);
    virtual void clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t c = 0);

    virtual void refresh();
    virtual void invertDisplay(bool invert);
    virtual void setCursor(uint8_t x, uint8_t y);
    virtual void write(uint8_t x, uint8_t y, uint8_t b);
    virtual void scroll(int8_t n);

};

#endif // SCREEN__H