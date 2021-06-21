#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
//#include <Fonts/TomThumb.h>
#include "handlers.h"
#include "const.h"




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



unsigned char b;

void loop()
{
  // read the incoming bytes:
  const char *toSend = 0;
  while (Serial.available())
  {
    b = Serial.read();
    uint8_p change = STATE_TABLE[parser.state - 1][b];
    Serial.print("==> ");
    Serial.println(parser.state - 1);
    Serial.print("==> ");
    Serial.println(b);
    Serial.print("==> ");
    Serial.println(STATE_TABLE[parser.state - 1][b]);
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