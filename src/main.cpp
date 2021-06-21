#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <vtparse.h>
//#include <Fonts/TomThumb.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FONT_WIDTH 5
#define FONT_HEIGHT 8 // For now use always 8, until I implement scrolling for other heights

#define SCREEN_COLS SCREEN_WIDTH / FONT_WIDTH
#define SCREEN_ROWS SCREEN_HEIGHT / FONT_HEIGHT

int to_print[2];

byte cursor_x = 0;
byte cursor_y = 0;

void draw_byte(char b);
void csi_dispatch(char b);

void parser_callback(vtparse_t *parser, vtparse_action_t action, unsigned char ch)
{
  Serial.println(ACTION_NAMES[action]);

  switch (action)
  {
  case VTPARSE_ACTION_PRINT:
    draw_byte(ch);
    break;
  case VTPARSE_ACTION_CSI_DISPATCH:
    csi_dispatch(ch);
    break;
  default:
    break;
  };
}

void csi_dispatch(char b)
{
  Serial.println(b);
}

vtparse_t parser;

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(50);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.display();
  display.clearDisplay();
  //  display.setFont(&TomThumb);
  display.setTextColor(1, 0);
  display.setTextWrap(0);
  Serial.write('\n');
  pinMode(A0, INPUT_PULLUP);
  // Init VT parser
  vtparse_init(&parser, parser_callback);
}

void scroll_8px(void)
{
  uint8_t *buf = display.getBuffer();
  // Copy each row of bytes up
  memcpy(buf, buf + 128, 128 * 7);
  // Blank the last line
  memset(buf + 128 * 7, 0, 128);
}

void draw_byte(char b)
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
    // display.print((char *)to_print);
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

unsigned char b;

void loop()
{
  // read the incoming bytes:
  const char *toSend = 0;
  while (Serial.available())
  {
    b = Serial.read();
    uint8_p change = STATE_TABLE[parser.state - 1][b];
    do_state_change(&parser, change, b);
    display.display();
  }

  // One-pin kbd handler
  switch (analogRead(A0))
  {
  case 105 ... 800:
    toSend = "\r\n";
    break;
  case 75 ... 104:
    toSend = "r";
    break;
  case 50 ... 74:
    toSend = "e";
    break;
  case 30 ... 49:
    toSend = "w";
    break;
  case 0 ... 29:
    toSend = "q";
    break;
  }
  if (toSend)
  {
    Serial.print(toSend);
    delay(500); // Dummy debounce
  }
}