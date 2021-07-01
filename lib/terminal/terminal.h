#ifndef TERMINAL__H
#define TERMINAL__H
#include <vtparse.h>

#include "kbd.h"
#include "screen.h"
#include "const.h"

class Terminal
{
public:
    Terminal(Keyboard *kbd, Screen *scr)
    {
        keyboard = kbd;
        screen = scr;
    };

    void init();
    // Cursor position (1,1 -> SCREEN_COLS, SCREEN_ROWS)
    uint8_t cursor_x = 0;
    uint8_t cursor_y = 0;
    // Saved cursor position for DECSC/DECRC
    uint8_t saved_cursor_x = 0;
    uint8_t saved_cursor_y = 0;

    // State machine to handle terminal input from host
    vtparse_t parser;

    // Keyboard for user input
    Keyboard *keyboard;
    // Screen for display
    Screen *screen;

    // Terminal state flags. Terminal may behave
    // differently based on these flags

    bool lnm = false;       // false is "reset/line feed"
                            // true is "set/new line"
    bool cursor_on = false; // Cursor blinks on/off

    // Timer that checks input/output and reacts to events
    void tick(void);
    unsigned long kbd_tock = 0;
    unsigned long serial_tock = 0;
    unsigned long cursor_tock = 0;

    // Time-triggered callbacks
    void read_kbd();
    void read_serial();
    void cursor_blink();

    // State machine handlers
    void process(uint8_t c);
    void do_action(vtparse_action_t action, uint8_t ch);
    void do_state_change(uint8_t change, uint8_t ch);
    void parser_callback(vtparse_action_t action, uint8_t ch);

    // Action handlers
    void handle_csi_dispatch(uint8_t b);
    void handle_esc_dispatch(uint8_t b);
    void handle_print(uint8_t b);
    void handle_execute(uint8_t b);

    // Utility functions
    void clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t c = 0);
    void scroll(uint8_t rows);
    void refresh(void);

    // Debugging convenience only
    void process_string(const char s[]);

    // Config
    bool kbd_enabled = true;
};

#endif // TERMINAL__H