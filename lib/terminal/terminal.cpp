#include <Arduino.h>
#include <ArduinoLog.h>
#include <avr/pgmspace.h>

#include "terminal.h"

Terminal::Terminal()
{
    //Clear screen buffer
    memset(screen, 0, SCREEN_COLS * SCREEN_ROWS);

    // Prepare 1-pin kbd
    pinMode(A0, INPUT_PULLUP);
    // Init VT parser
    parser.state = VTPARSE_STATE_GROUND;
    parser.num_intermediate_chars = 0;
    parser.num_params = 0;
    parser.ignore_flagged = 0;

    // Initialize screen
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
    oled.setFont(Adafruit5x7);
    oled.clear();
}

void Terminal::process(uint8_t c)
{
    uint8_t change = pgm_read_byte(STATE_TABLE + (parser.state - 1) * 160 + c);
    do_state_change(change, c);
}

void Terminal::read_kbd()
{
    // Read keyboard event, put data in kbd_buffer

    // One-pin kbd handler
    switch (analogRead(A0))
    {
    case 105 ... 800:
        strncpy(kbd_buffer, "\r\n\0", 3);
        break;
    case 75 ... 104:
        strncpy(kbd_buffer, "r\0", 2);
        break;
    case 50 ... 74:
        strncpy(kbd_buffer, "e\0", 2);
        break;
    case 30 ... 49:
        strncpy(kbd_buffer, "w\0", 2);
        break;
    case 0 ... 29:
        strncpy(kbd_buffer, "q\0", 2);
        break;
    }
    delay(200); // FIXME: Dummy debounce
}

void Terminal::parser_callback(vtparse_action_t action, uint8_t ch)
{
    int i;
    Log.infoln("ACTION: %s '%c'\r", ACTION_NAMES[action], ch);
    Log.infoln("%d Intermediate chars:\r", parser.num_intermediate_chars);
    for (i = 0; i < parser.num_intermediate_chars; i++)
    {
        Log.infoln("\t0x%02x ('%c')\r", parser.intermediate_chars[i], parser.intermediate_chars[i]);
    }
    Log.infoln("%d Parameters:\r", parser.num_params);
    for (i = 0; i < parser.num_params; i++)
    {

        Log.infoln("\t%d\r", parser.params[i]);
    }
    switch (action)
    {
    case VTPARSE_ACTION_PRINT:
        handle_print(ch);
        break;
    case VTPARSE_ACTION_CSI_DISPATCH:
        handle_csi_dispatch(ch);
    default:
        break;
    };
}

void Terminal::do_state_change(uint8_t change, uint8_t ch)
{
    /* A state change is an action and/or a new state to transition to. */

    vtparse_state_t new_state = (vtparse_state_t)STATE(change);
    vtparse_action_t action = (vtparse_action_t)ACTION(change);

    if (new_state)
    {
        /* Perform up to three actions:
         *   1. the exit action of the old state
         *   2. the action associated with the transition
         *   3. the entry action of the new state
         */

        vtparse_action_t exit_action = EXIT_ACTIONS[parser.state - 1];
        vtparse_action_t entry_action = ENTRY_ACTIONS[new_state - 1];

        if (exit_action)
            do_action(exit_action, 0);

        if (action)
            do_action(action, ch);

        if (entry_action)
            do_action(entry_action, 0);

        parser.state = new_state;
    }
    else
    {
        do_action(action, ch);
    }
}

void Terminal::do_action(vtparse_action_t action, uint8_t ch)
{
    /* Some actions we handle internally (like parsing parameters), others
     * we hand to our client for processing */

    switch (action)
    {
    case VTPARSE_ACTION_PRINT:
    case VTPARSE_ACTION_EXECUTE:
    case VTPARSE_ACTION_HOOK:
    case VTPARSE_ACTION_PUT:
    case VTPARSE_ACTION_OSC_START:
    case VTPARSE_ACTION_OSC_PUT:
    case VTPARSE_ACTION_OSC_END:
    case VTPARSE_ACTION_UNHOOK:
    case VTPARSE_ACTION_CSI_DISPATCH:
    case VTPARSE_ACTION_ESC_DISPATCH:
        parser_callback(action, ch);
        break;

    case VTPARSE_ACTION_IGNORE:
        /* do nothing */
        break;

    case VTPARSE_ACTION_COLLECT:
    {
        /* Append the character to the intermediate params */
        if (parser.num_intermediate_chars + 1 > MAX_INTERMEDIATE_CHARS)
            parser.ignore_flagged = 1;
        else
            parser.intermediate_chars[parser.num_intermediate_chars++] = ch;

        break;
    }

    case VTPARSE_ACTION_PARAM:
    {
        /* process the param character */
        if (ch == ';')
        {
            parser.num_params += 1;
            parser.params[parser.num_params - 1] = 0;
        }
        else
        {
            /* the character is a digit */
            int current_param;

            if (parser.num_params == 0)
            {
                parser.num_params = 1;
                parser.params[0] = 0;
            }

            current_param = parser.num_params - 1;
            parser.params[current_param] *= 10;
            parser.params[current_param] += (ch - '0');
        }

        break;
    }

    case VTPARSE_ACTION_CLEAR:
        parser.num_intermediate_chars = 0;
        parser.num_params = 0;
        parser.ignore_flagged = 0;
        break;

    default:
        parser_callback(VTPARSE_ACTION_ERROR, 0);
    }
}

void Terminal::handle_csi_dispatch(uint8_t b)
{
    switch (b)
    {
    case 'K':                                       // Erase line
    case 'J':                                       // Erase screen
        if (parser.num_params and parser.params[0]) // Should be only 0 or 1
        {                                           // Assume 1 param
            if (parser.params[0] == 1)
            {
                clear(0, cursor_x, cursor_y, cursor_y); // Clear from start of line
                if (b == 'J')
                { // Also clear from start of screen
                    clear(0, SCREEN_COLS, 0, cursor_y - 1);
                }
            }
            else
            {                                              // Assume param is 2
                clear(0, SCREEN_COLS, cursor_y, cursor_y); // Clear whole line
                if (b == 'J')
                {
                    oled.clear(); // Clear whole screen
                }
            }
        }
        else // Assume 0 params or 1 param that's 0
        {    // Erase to EOL
            if (b == 'J')
            {
                // Also clear to end of screen
                clear(0, SCREEN_COLS, cursor_y + 1, SCREEN_ROWS);
            }
            else
            {
                clear(cursor_x, SCREEN_COLS, cursor_y, cursor_y); // Clear to EOL
            }
        }
        break;
    default:
        Log.infoln("Unknown CSI character %c\r", b);
    }
}

void Terminal::clear(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
    oled.clear(x1 * FONT_W_PX, x2 * FONT_W_PX, y1, y2);
}

void Terminal::handle_print(uint8_t b)
{
    // Handle Printable and control characters, see
    // https://vt100.net/docs/vt100-ug/chapter3.html
    switch (b)
    {
    case 7: // BEL
        // TODO
        break;
    case 8: // BS
        // Clear cursor
        // display.fillRect(cursor_x * FONT_W_PX, cursor_y * 8, FONT_W_PX, 8, 0);
        if (cursor_x > 0)
            cursor_x--;
        break;
    case 9: // HT
        // TODO
        break;
    case 10: // LF
    case 11:
    case 12:
    case 13: // CR
        // Clear cursor
        // display.fillRect(cursor_x * FONT_W_PX, cursor_y * 8, FONT_W_PX, 8, 0);
        cursor_x = 0;
        cursor_y++;
        break;
    case 14: // SO
        // TODO
        break;
    case 15: // SI
        // TODO
        break;
    case 17: // XON
        // TODO
        break;
    case 18: // XOFF
        // TODO
        break;
    case 24: // CAN
    case 26:
        // TODO
        break;
    case 127: // DEL
        break;
    default: // Printable
        // display.fillRect(cursor_x * FONT_W_PX, cursor_y * 8, FONT_W_PX, 8, 0);
        oled.setCursor(cursor_x * FONT_W_PX, cursor_y);
        oled.write(b);
        screen[cursor_x][cursor_y] = b;
        // Advance cursor
        cursor_x += 1;
    }

    // Wrap / Scroll
    if (cursor_x == SCREEN_COLS)
    {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y == SCREEN_ROWS)
    {
        oled.scrollDisplay(8);
        cursor_y = SCREEN_ROWS - 1;
    }
    // Draw the cursor
    oled.setCursor(cursor_x * FONT_W_PX, cursor_y);
    oled.write(178);
}
