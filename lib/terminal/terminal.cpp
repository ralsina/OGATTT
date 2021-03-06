#include <Arduino.h>
#include <ArduinoLog.h>

#ifndef NATIVE
#include <avr/pgmspace.h>
#endif

#include "terminal.h"

#include "kbd.h"
#include "screen.h"
#include "keymap.h"

#define LED 13
#define BUZZER 13

#define MAX(a, b) ((a) > (b) ? a : b)
#define MIN(a, b) ((a) < (b) ? a : b)

void (*resetFunc)(void) = 0;

void Terminal::init()
{
    // Init internal state
    cursor_x = 1;
    cursor_y = 1;
    saved_cursor_x = 1;
    saved_cursor_y = 1;
    kbd_tock = 0;
    serial_tock = 0;
    lnm = false;
    cursor_on = false;
    keyboard->init();
    screen->init();

    // LED is Visual Bell, BUZZER is the Bell.
    if (LED)
        pinMode(LED, OUTPUT);
    if (BUZZER)
        pinMode(BUZZER, OUTPUT);

    // Init VT parser
    parser.state = VTPARSE_STATE_GROUND;
    parser.num_intermediate_chars = 0;
    parser.num_params = 0;
    parser.ignore_flagged = 0;

    // Initialize keyboard
}

void Terminal::tick()
{
    unsigned long t = MAX(micros(), serial_tock); // micros wraps every 70 days
    if (t - serial_tock > 1000)                   // 1 msec
    {
        read_serial();
        serial_tock = t;
    }

    if (kbd_enabled && (t - kbd_tock > 10000)) // 10 msec
    {
        read_kbd();
        kbd_tock = t;
    }

    if (t - cursor_tock > 500000) // half a second
    {
        cursor_blink();
        cursor_tock = t;
    }
}

void Terminal::cursor_blink()
{
    // Since the screen doesn't clear a character when displaying it, we can just
    // Draw a _ to blink ON, and the screen character to blink OFF
    if (cursor_on)
    {
        cursor_on = false;
        screen->write(cursor_x, cursor_y, 178);
    }

    else
    {
        cursor_on = true;
        screen->write(cursor_x, cursor_y, _screen[cursor_x][cursor_y]);
    }
}

void Terminal::read_kbd()
{
    uint8_t kc = keyboard->get_key();
    if (kc != 255)
    {
        uint8_t r = kc / 16;
        uint8_t c = kc % 16;
        process(plain[12 * r + c]);
    }
}

void Terminal::read_serial()
{
    // read the incoming bytes:
    while (Serial.available())
    {
        process(Serial.read());
    }
}

void Terminal::process(uint8_t c)
{
    Log.infoln("Processing: %d\r", c);
    uint8_t change = pgm_read_byte(STATE_TABLE + (parser.state - 1) * 160 + c);
    do_state_change(change, c);
}

void Terminal::process_string(const char s[])
{
    for (int i = 0; s[i]; i++)
    {
        process(s[i]);
    }
}

void Terminal::parser_callback(vtparse_action_t action, uint8_t ch)
{
    int i;
    Log.infoln("ACTION: %s -- %d '%c'\r", ACTION_NAMES[action], ch, ch);
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
    case VTPARSE_ACTION_EXECUTE:
        handle_execute(ch);
        break;
    case VTPARSE_ACTION_CSI_DISPATCH:
        handle_csi_dispatch(ch);
        break;
    case VTPARSE_ACTION_ESC_DISPATCH:
        handle_esc_dispatch(ch);
        break;
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
    // Shortcut for sequences with parameters
    uint8_t p0 = parser.num_params > 0 ? parser.params[0] : 0;
    uint8_t p1 = parser.num_params > 1 ? parser.params[1] : 0;

    switch (b)
    {
    case 'A': // CUU - Cursor Up
        cursor_y = MAX(0, cursor_y - MAX(1, p0));
        break;
    case 'B': // CUD - Cursor Down
        cursor_y = MIN(SCREEN_ROWS, cursor_y + MAX(1, p0));
        break;
    case 'C': // CUF - Cursor Forward
        cursor_x = MIN(SCREEN_COLS, cursor_x + MAX(1, p0));
        break;
    case 'D': // CUB - Cursor Backwards
        cursor_x = MAX(1, cursor_x - MAX(1, p0));
        break;

    case 'f': // HVP
    case 'H': // CUP ??? Cursor Position
        cursor_x = MIN(MAX(p1, 1), SCREEN_COLS);
        cursor_y = MIN(MAX(p0, 1), SCREEN_ROWS);
        break;

    case 'J': // ED - Erase In Display
        switch (p0)
        {
        case 0: // Clear to end of screen
            screen->clear(1, SCREEN_COLS, cursor_y + 1, SCREEN_ROWS);
            break;
        case 1: // Clear from start of screen
            screen->clear(1, SCREEN_COLS, 1, cursor_y - 1);
            break;
        default: // 2 Clear whole screen
            screen->clear(1, SCREEN_COLS, 1, SCREEN_ROWS);
        }     // Intentional no break
    case 'K': // EK - Erase In Line
        switch (p0)
        {
        case 0: // Clear to EOL
            screen->clear(cursor_x, SCREEN_COLS, cursor_y, cursor_y);
            break;
        case 1: // Clear from start of line
            screen->clear(1, cursor_x, cursor_y, cursor_y);
            break;
        default: // 2  Clear whole line
            screen->clear(1, SCREEN_COLS, cursor_y, cursor_y);
        }
        break;
    case 'c': // Device attributes (TODO)
        break;
    case 'h':
    case 'l':
        switch (parser.intermediate_chars[0])
        {
        case '2':
            lnm = b == 'h';
            break;
        case '?':
            switch (p0)
            {
            case 5:
                // DECSCNM reverse video
                screen->invertDisplay(b == 'h');
                break;
            }
            break;
        }
        if (parser.intermediate_chars[0] == '?')
        {
        }
        break;
    case 'n': // Reports
        switch (p0)
        {
        case 6:                    // CPR - Cursor position report
            break;                 // TODO, reply with ESC [ cursor_y; cursor_x R
        case 5:                    // Status Report
            Serial.write("\e[0n"); // Terminal ok
            break;
        }
        break;
    case 'q': // DECLL - Load LEDS (DEC Private)
        // Can't implement because I am using the onboard LED
        // pin for BEL and am out of pins.
        break;
    default:
        Log.infoln("Unknown CSI character %c\r", b);
    }
}

void Terminal::handle_esc_dispatch(uint8_t b)
{
    switch (b)
    {
    case 'c': // RIS - Reset To Initial State
        resetFunc();
    case '7': // DECSC - Save Cursor
        saved_cursor_x = cursor_x;
        saved_cursor_y = cursor_y;
        break;
    case '8':
        if (parser.intermediate_chars[0] == '#') // DECALN (WTF)
        {
            // This command fills the entire screen area with uppercase Es for
            // screen focus and alignment. This command is used by DEC manufacturing
            // and Field Service personnel.
            screen->clear(0, SCREEN_COLS, 0, SCREEN_ROWS, 'E');
        }
        else // DECRC - Restore cursor
        {
            cursor_x = saved_cursor_x;
            cursor_y = saved_cursor_y;
        }
        break;
    case 'D': // IND - Index
        cursor_y++;
        break;
    case 'E': // NEL - Next Line
        cursor_y++;
        cursor_x = 1;
        break;
    default:
        Log.infoln("Unknown ESC_DISPATCH character %c\r", b);
    }
}

void Terminal::handle_print(uint8_t b)
{
    // Handle Printable and control characters, see
    // https://vt100.net/docs/vt100-ug/chapter3.html

    // display.fillRect(cursor_x * FONT_W_PX, cursor_y * 8, FONT_W_PX, 8, 0);
    _screen[cursor_x][cursor_y] = b;
    screen->write(cursor_x, cursor_y, b);
    // Advance cursor
    cursor_x++;

    // Wrap / Scroll
    if (cursor_x > SCREEN_COLS)
    {
        cursor_x = 1;
        cursor_y++;
    }
    if (cursor_y > SCREEN_ROWS)
    {
        screen->scroll(1);
        cursor_y = SCREEN_ROWS;
    }
}

void Terminal::handle_execute(uint8_t b)
{
    switch (b)
    {
    case 0: // NUL Ignored on input (not stored in input buffer; see full duplex protocol).
        break;
    case 5: // ENQ
        Serial.write("Hi There");
        break;
    case 7: // BEL
        // FIXME do it without the delay using the timer loop
        tone(BUZZER, 200);
        delay(100);
        noTone(BUZZER);
    case 8: // BS
        cursor_x = MAX(1, cursor_x - 1);
        break;
    case 9: // HT FIXME: Fixed tabs at multiples of 8
        cursor_x = MIN(8 * (((cursor_x + 1) / 8) + 1), SCREEN_COLS);
    case 10: // LF
    case 11: // VT
    case 12: // FF
        cursor_y += 1;
        if (lnm)
        {
            cursor_x = 1;
        }
        break;
    case 13: // CR
        cursor_x = 1;
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
    case 26: // SUB
        // If sent during a control sequence, the sequence is immediately terminated
        // and not executed. It also causes the error character to be displayed.
        //
        // My guess is this should be handled by the FSM already so not implementing
        break;
    case 127: // DEL
        break;
    default:
        Log.infoln("Unknown execute code: %d\r", b);
    }
}
