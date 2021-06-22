#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_BUFFER_ROWS SCREEN_HEIGHT / 8  // "Rows of bytes" in the screen buffer

#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define FONT_WIDTH 5
#define FONT_HEIGHT 8 // For now use always 8, until I implement scrolling for other heights

#define SCREEN_COLS SCREEN_WIDTH / FONT_WIDTH
#define SCREEN_ROWS SCREEN_HEIGHT / FONT_HEIGHT
