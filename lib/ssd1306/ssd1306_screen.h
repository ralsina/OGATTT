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
    virtual void init();

    virtual void clear();
    virtual void clear(uint8_t c);
    virtual void clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t c = 0);

    virtual void refresh();
    virtual void invertDisplay(bool invert);
    virtual void setCursor(uint8_t x, uint8_t y);
    virtual void write(uint8_t x, uint8_t y, uint8_t b);
};

#endif // SDD1306SCREEN__H