#include <stdint.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoLog.h>

#include "handlers.h"
#include "const.h"

vtparse_t parser;

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void csi_dispatch(char b)
{
    Serial.println(b);
}

void scroll_8px(void)
{
    uint8_t *buf = display.getBuffer();
    // Copy each row of bytes up
    memcpy(buf, buf + 128, 128 * 7);
    // Blank the last line
    memset(buf + 128 * 7, 0, 128);
}

char to_print[2];

void handle_print(char b)
{
    to_print[0] = b;
    to_print[1] = 0;

    // Handle Printable and control characters, see
    // https://vt100.net/docs/vt100-ug/chapter3.html
    switch (b)
    {
    case 7: // BEL
        // TODO
        break;
    case 8: // BS
        if (cursor_x > 0)
            cursor_x--;
        break;
    case 9: // HT
        // TODO
        break;
    case 10: // LF
    case 11:
    case 12:
        cursor_x = 0;
        cursor_y++;
        break;
    case 13: // CR
        cursor_x = 0;
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
        display.setCursor(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT);
        display.fillRect(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, 0);
        display.print((char *)to_print);
        // Serial.print((char *)to_print);
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
        scroll_8px();
        cursor_y = SCREEN_ROWS - 1;
    }
    // Draw the cursor
    display.fillRect(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, 1);
}

void parser_callback(vtparse_t *parser, vtparse_action_t action, unsigned char ch)
{
    Serial.print("ACTION --> ");
    Serial.println(ACTION_NAMES[action]);

    switch (action)
    {
    case VTPARSE_ACTION_PRINT:
        handle_print(ch);
        break;
    case VTPARSE_ACTION_CSI_DISPATCH:
        csi_dispatch(ch);
        break;
    default:
        break;
    };
}

void read_kbd(char *data)
{
    // One-pin kbd handler
    switch (analogRead(A0))
    {
    case 105 ... 800:
        strncpy(data, "\r\n\0", 3);
        break;
    case 75 ... 104:
        strncpy(data, "r\0", 2);
        break;
    case 50 ... 74:
        strncpy(data, "e\0", 2);
        break;
    case 30 ... 49:
        strncpy(data, "w\0", 2);
        break;
    case 0 ... 29:
        strncpy(data, "q\0", 2);
        break;
    }
    delay(200); // Dummy debounce
}

void init_terminal(void)
{
    // Prepare 1-pin kbd
    pinMode(A0, INPUT_PULLUP);
    // Init VT parser
    vtparse_init(&parser, parser_callback);
}