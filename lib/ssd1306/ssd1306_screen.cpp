#include <ArduinoLog.h>
#include "ssd1306_screen.h"

void SSD1306Screen::init()
{
    Log.infoln("SSD1306::init\r");

    screen.begin(&Adafruit128x64, I2C_ADDRESS);
    screen.setFont(Adafruit5x7);
    Screen::init();
};

void SSD1306Screen::clear()
{
    Screen::clear();
}

void SSD1306Screen::clear(uint8_t c)
{
    Screen::clear(c);
}

void SSD1306Screen::clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t c)
{
    Screen::clear(x1, x2, y1, y2, c);
    if (c)
        refresh();
    else
        screen.clear((x1 - 1) * FONT_W_PX, (x2)*FONT_W_PX - 1, y1 - 1, y2 - 1);
};
void SSD1306Screen::invertDisplay(bool invert)
{
    screen.invertDisplay(invert);
};

void SSD1306Screen::refresh(void)
{
    screen.clear();
    for (uint8_t x = 0; x < SCREEN_COLS; x++)
    {
        for (uint8_t y = 0; y < SCREEN_ROWS; y++)
        {
            screen.setCursor(x * FONT_W_PX, y);
            screen.write(_screen[x + 1][y + 1] ? _screen[x + 1][y + 1] : ' ');
        }
    }
}

void SSD1306Screen::write(uint8_t x, uint8_t y, uint8_t b)
{
    setCursor(x, y);
    screen.write(b ? b : ' ');
    Screen::write(x, y, b);
}

void SSD1306Screen::setCursor(uint8_t x, uint8_t y)
{
    screen.setCursor((x - 1) * FONT_W_PX, y - 1);
}
