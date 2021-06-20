#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

#define BUF_SIZE 128
char inputBuffer[BUF_SIZE]; // for incoming serial data
int to_print[2];

byte cursor_x = 0;
byte cursor_y = 0;

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
}

void scroll_8px(void)
{
  uint8_t *buf = display.getBuffer();
  // Just copy each row of bytes up and blank the last one.
  //
  memcpy(buf, buf + 128, 128 * 7);
  memset(buf + 128 * 7, 0, 128);
}

void draw_byte(char b)
{
  to_print[0] = b;
  to_print[1] = 0;

  display.setCursor(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT);
  display.fillRect(cursor_x * FONT_WIDTH, cursor_y * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, 0);
  display.print((char *)to_print);
  //  Serial.print((char *)to_print);

  // Advance cursor
  cursor_x += 1;
  if (b == 13 || cursor_x == SCREEN_COLS)
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

// int received = 0;
void loop()
{
  // read the incoming bytes:
  const char *toSend = 0;
  int received = 0;
  while (Serial.available())
  {
    draw_byte(Serial.read());
    received = 1;
  }
  if (received)
  {
    display.display();
  }

  // One-pin kbd handler
  received = analogRead(A0);
  switch (received)
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