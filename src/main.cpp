#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <ArduinoLog.h>
#include "handlers.h"
#include "const.h"

void setup()
{
  // // Initialize serial port
  Serial.begin(9600);
  while (!Serial && !Serial.available())
  {
  };
  Serial.setTimeout(50);

  // Initialize screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
    }
  }

  // Graphical details
  display.setTextColor(1, 0);
  display.setTextWrap(0);
  display.fillScreen(0);
  display.display();

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.infoln("LOGGING HERE");
  display.display();
  delay(500);
  display.fillScreen(0);
  display.display();

  init_terminal();
}

uint8_t b;
char to_serial[3];

void loop()
{
  // read the incoming bytes:
  while (Serial.available()>0)
  {
    b = Serial.read();
    // Log.infoln("state--> %d b---> %d", parser.state, b);
    uint8_t change = pgm_read_byte(STATE_TABLE[parser.state - 1][b]);
    do_state_change(&parser, change, b);
    display.display();
  }

  memset(to_serial, 0, 3);
  read_kbd(to_serial);
  if (to_serial[0])
  {
    Serial.write(to_serial, strlen(to_serial));
  }
}