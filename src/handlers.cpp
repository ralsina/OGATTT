#include <stdint.h>
#include <string.h>
#include "SSD1306AsciiAvrI2c.h"
#include <ArduinoLog.h>

#include "handlers.h"
#include "const.h"
#include "terminal.h"

Terminal terminal;


uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

extern SSD1306AsciiAvrI2c oled;

void read_kbd(char *data)
{
    // One-pin kbd handler
    switch (analogRead(A0))
    {
    case 105 ... 800:
        strncpy(data, "\r\n\0", 3);
        break;
    case 75 ... 104:
        strncpy(data, "r\0", 2);
        break;
    case 50 ... 74:
        strncpy(data, "e\0", 2);
        break;
    case 30 ... 49:
        strncpy(data, "w\0", 2);
        break;
    case 0 ... 29:
        strncpy(data, "q\0", 2);
        break;
    }
    delay(200); // Dummy debounce
}
