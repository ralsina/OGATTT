#include <Arduino.h>
#include <ArduinoLog.h>
#include <avr/pgmspace.h>

#include "terminal.h"

Terminal::Terminal()
{
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

void Terminal::process(unsigned char c)
{
    uint8_t change = pgm_read_byte(STATE_TABLE + (parser.state - 1) * 160 + c);
    do_state_change(change, c);
}

void Terminal::parser_callback(vtparse_action_t action, unsigned char ch)
{
    Log.infoln("ACTION: %s C: %d\r", ACTION_NAMES[action], ch);

    switch (action)
    {
    case VTPARSE_ACTION_PRINT:
        handle_print(ch);
        break;
    default:
        break;
    };
}

void Terminal::do_state_change(uint8_t change, char ch)
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

void Terminal::do_action(vtparse_action_t action, char ch)
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

void Terminal::handle_print(char b)
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
