#include <Arduino.h>
#include <ArduinoLog.h>

#include "kbd.h"

#define KBD_COLS 1
#define KBD_ROWS 1

// Actual wiring
uint8_t kbd_cols[] = {2, 3};
// uint8_t kbd_rows[] = {20, 4, 21};
uint8_t kbd_rows[] = { 20};

#define KEYCODE(c, r) (c << (4 + r))

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
        // pinMode(kbd_rows[i], INPUT_PULLUP);
        pinMode(kbd_rows[i], INPUT_PULLUP);
    }

}

const char *Keyboard::get_key()
{
    // According to the vt100 User Guide:

    // Auto repeating works as follows: when a key is typed, its code(s) is sent once,
    // immediately. If the key is held down for more than ½ second, the code(s) will be
    // sent repeatedly at a rate of approximately 30 Hz (less if low transmit baud rates
    // are used) until the key is released.

    // So, we should scan the kbd every 10msec, send any NEW keys, then
    // if there was > 500msec from pressing and still pressed, start sending it
    // every 33msec

    // Read keyboard event, put data in kbd_buffer


    digitalWrite(2, LOW);
    pinMode(20, INPUT);
    Log.infoln("--> %d\r", analogRead(20));


    // int x =0;
    // for (uint8_t c = 0; c < KBD_COLS; c++)
    // {
    //     Log.infoln("Col: %d\r", kbd_cols[c]);
    //     digitalWrite(kbd_cols[c], LOW);
    //     for (uint8_t r = 0; r < KBD_ROWS; r++)
    //     {
    //         x = digitalRead(kbd_rows[r]);
    //         Log.infoln("Row %d %d\r", kbd_rows[r], x);
    //     }
    //     digitalWrite(kbd_cols[c], HIGH);
    // }
    return 0;
}