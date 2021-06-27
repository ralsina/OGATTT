#include <Arduino.h>
#include <ArduinoLog.h>

#include "kbd.h"

#define KBD_COLS 12
#define KBD_ROWS 5

// Actual wiring
uint8_t kbd_cols[] = {2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 16, 17};

uint8_t kbd_rows[] = {15, 14, 20, 4, 21};

#define KEYCODE(r, c) (c + r * 16)

void Keyboard::init()
{
    // This is a matrix keyboard, setup pins
    for (uint8_t i = 0; i < KBD_COLS; i++)
    {
        pinMode(kbd_cols[i], OUTPUT);
        digitalWrite(kbd_cols[i], HIGH);
    }
    for (uint8_t i = 0; i < KBD_ROWS; i++)
    {
        switch (kbd_rows[i])
        {
        case 20:
        case 21:
            break;
        default:
            pinMode(kbd_rows[i], INPUT_PULLUP);
            break;
        }
    }
}

uint8_t Keyboard::get_key()
{
    // According to the vt100 User Guide:

    // Auto repeating works as follows: when a key is typed, its code(s) is sent once,
    // immediately. If the key is held down for more than ½ second, the code(s) will be
    // sent repeatedly at a rate of approximately 30 Hz (less if low transmit baud rates
    // are used) until the key is released.

    // So, we should scan the kbd every 10msec, send any NEW keys, then
    // if there was > initial_delay from pressing and still pressed, start sending it
    // every repeat_delay

    // FIXME: keep internal list of "pressed keys" instead of returning
    // the keycode

    bool x = false;
    uint8_t keycode = 255;
    bool emit = false;
    unsigned long delta = micros() - last_emitted_time;
    for (uint8_t c = 0; c < KBD_COLS; c++)
    {
        digitalWrite(kbd_cols[c], LOW);
        for (uint8_t r = 0; r < KBD_ROWS; r++)
        {
            switch (kbd_rows[r])
            {
            case 20:
            case 21:
                x = analogRead(kbd_rows[r]) < 500;
                break;
            default:
                x = !digitalRead(kbd_rows[r]);
                break;
            }
            if (x)
            {
                emit = false;
                keycode = KEYCODE(c, r);
                if (keycode == last_emitted_keycode)
                {
                    if ((delta > initial_delay) || (delta > repeat_delay) && repeating)
                    {
                        emit = true;
                        repeating = true;
                    }
                }
                else // Different key
                {
                    repeating = false;
                    emit = true;
                }
                if (emit)
                {
                    last_emitted_keycode = keycode;
                    last_emitted_time = micros();
                    digitalWrite(kbd_cols[c], HIGH);
                    return keycode;
                }
            }
        }
        digitalWrite(kbd_cols[c], HIGH);
    }
    if (delta > repeat_delay) // No key emitted for a while
        repeating = false;
    return 255;
}