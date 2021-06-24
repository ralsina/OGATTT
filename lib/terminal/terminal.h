#ifndef TERMINAL__H
#define TERMINAL__H
#include <SSD1306AsciiAvrI2c.h>
#include <vtparse.h>
#include "const.h"

class Terminal
{
public:
    Terminal();
    uint8_t screen[SCREEN_COLS][SCREEN_ROWS];
    char kbd_buffer[10];
    uint8_t cursor_x = 0;
    uint8_t cursor_y = 0;
    vtparse_t parser;
    SSD1306AsciiAvrI2c oled; // FIXME: maybe take as argument

    // I/O handlers
    void process(uint8_t c);
    void read_kbd();

    // State machine handlers
    void do_action(vtparse_action_t action, uint8_t ch);
    void do_state_change(uint8_t change, uint8_t ch);
    void parser_callback(vtparse_action_t action, uint8_t ch);

    // Action handlers
    void handle_csi_dispatch(uint8_t b);
    void handle_print(uint8_t b);
    void handle_execute(uint8_t b);

    // Utility functions
    void clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
    void scroll(uint8_t rows);
    void refresh(void);

    // Debugging only
    void process_string(const char s[]);  
};

#endif // TERMINAL__H