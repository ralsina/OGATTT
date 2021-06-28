#ifndef SDD1306SCREEN__H
#define SDD1306SCREEN__H

#include "screen.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

class SSD1306Screen: public Screen
/* Physical screen based on a SDD1306 */
{
public:
    SSD1306AsciiAvrI2c screen;
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
};

#endif // SDD1306SCREEN__H