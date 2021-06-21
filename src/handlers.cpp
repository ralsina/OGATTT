#include <stdint.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "handlers.h"
#include "const.h"

vtparse_t parser;

int to_print[2];

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

void handle_print(char b)
{
    to_print[0] = b;
    to_print[1] = 0;

    // Handle Printable and control characters, see
    // See https://vt100.net/docs/vt100-ug/chapter3.html
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
