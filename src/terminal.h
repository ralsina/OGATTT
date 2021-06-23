#ifndef TERMINAL__H
#define TERMINAL__H
#include <SSD1306AsciiAvrI2c.h>
#include <vtparse.h>
#include "const.h"

class Terminal
{
public:
    Terminal();
    char screen[SCREEN_COLS][SCREEN_ROWS];
    unsigned char cursor_x;
    unsigned char cursor_y;
    vtparse_t parser;
    SSD1306AsciiAvrI2c oled;

    void process(unsigned char c);

    void do_action(vtparse_action_t action, char ch);
    void do_state_change(uint8_t change, char ch);
    void handle_print(char b);
    void parser_callback(vtparse_action_t action, unsigned char ch);
};

#endif // TERMINAL__H