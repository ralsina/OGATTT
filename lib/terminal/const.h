#ifndef CONST__H
#define CONST__H

#define SCREEN_WIDTH 128                     // OLED display width, in pixels
#define SCREEN_HEIGHT 64                     // OLED display height, in pixels
#define SCREEN_BUFFER_ROWS SCREEN_HEIGHT / 8 // "Rows of bytes" in the screen buffer

#define OLED_RESET 4     // Reset pin # (or -1 if sharing Arduino reset pin)
#define I2C_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define RST_PIN -1       // Define proper RST_PIN if required.

#define FONT_W_PX 5
#define FONT_H_PX 8

#define SCREEN_COLS (SCREEN_WIDTH / FONT_W_PX)
#define SCREEN_ROWS (SCREEN_HEIGHT / 8)

#endif  // CONST__H