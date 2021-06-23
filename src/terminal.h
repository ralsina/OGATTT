#ifndef TERMINAL__H
#define TERMINAL__H
#include <SSD1306AsciiAvrI2c.h>
#include <vtparse.h>
#include "const.h"

class Terminal
{
public:
    Terminal();
    // char screen[SCREEN_COLS][SCREEN_ROWS];
    char kbd_buffer[10];
    uint8_t cursor_x;
    uint8_t cursor_y;
    vtparse_t parser;
    SSD1306AsciiAvrI2c oled;  // FIXME: maybe take as argument

    void process(uint8_t c);
    void read_kbd();

    void do_action(vtparse_action_t action, uint8_t ch);
    void do_state_change(uint8_t change, uint8_t ch);
    void handle_print(uint8_t b);
    void parser_callback(vtparse_action_t action, uint8_t ch);
};

#endif // TERMINAL__H