#include <ArduinoLog.h>
#include "screen.h"
#include <string.h>
#include <stdint.h>

#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)
uint8_t _screen[SCREEN_COLS + 1][SCREEN_ROWS + 1];

void Screen::init()
{
    Log.infoln("Screen::init\r");
    clear();
};

void Screen::clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t c)
{
    for (int x = MAX(x1, 1); x <= MIN(x2, SCREEN_COLS); x++)
    {
        for (int y = MAX(y1, 1); y <= MIN(y2, SCREEN_ROWS); y++)
        {
            _screen[x][y] = c;
        }
    }
};

void Screen::clear()
{
    clear(0);
};

void Screen::clear(uint8_t c)
{
    clear(1, SCREEN_COLS, 1, SCREEN_ROWS, c);
};

void Screen::invertDisplay(bool invert){};

void Screen::refresh(void){};

void Screen::write(uint8_t x, uint8_t y, uint8_t b)
{
    _screen[x][y] = b;
}

void Screen::setCursor(uint8_t x, uint8_t y){};

void Screen::scroll(int8_t n)
{
    if (n < 1)
        // TODO Implement scroll DOWN
        return;
    if (n >= SCREEN_ROWS)
    {
        clear();
        return;
    }

    for (uint8_t x = 1; x <= SCREEN_COLS; x++)
    {
        memmove(_screen[x] + 1,
                _screen[x] + n + 1,
                SCREEN_ROWS - n);
        memset(_screen[x] + 1 + SCREEN_ROWS - n, 0, n);
    }
    refresh(); // FIXME: see if it can be made to work with scrollDisplay
}
