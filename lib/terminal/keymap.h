#ifndef KEYMAP_H
#define KEYMAP_H

#include <stdint.h>

/* A basic keymap.

The layout will be weird because of our lack of keys (60) and the odd
size of the rows (12). This is NOT the proper layout for a vt100, since
it will have things like ALT and not have things like LINE FEED.

WARNING: the names of things here IS PROBABLY WRONG.

These tables map the position of the key (the keycode) to
a symbol or a modifier.

Modifiers don't repeat, they just set a flag in the keyboard
when pressed, and unset it when released.

Modifiers are: */

#define SHIFT 200
#define CTRL 201
#define ALT 202

/* Cursor keys */

#define LEFT 203
#define RIGHT 204
#define UP 205
#define DOWN 206

/* First table is plain symbols, what the key represents with no
modifiers */

const uint8_t plain[]{
    /* ROW 0  */ 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 8,   // - = moved to row 4
    /* ROW 1   */ 9, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 127, // [ ] moved to row 4, \ moved to row 3
    /* ROW 2 */ 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 13,
    /* ROW 3 */ 200, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\\',
    /* ROW 4 */ 201, 202, '[', ']', '-', '=', ' ', ' ', LEFT, UP, DOWN, RIGHT};

#endif // KEYMAP_H