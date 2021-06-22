#include <Arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <avr/pgmspace.h>
#include <ArduinoLog.h>
#include "handlers.h"
#include "const.h"


SSD1306AsciiAvrI2c oled;

void setup()
{
  // // Initialize serial port
  Serial.begin(9600);
  while (!Serial && !Serial.available())
  {
  };
  Serial.setTimeout(50);

  // Initialize screen
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();

  // Initialize logging
  Log.begin(LOG_LEVEL_VERBOSE, &oled);
  Log.infoln("LOGGING HERE");

  init_terminal();
}

uint8_t b;
char to_serial[3];

void loop()
{
  // read the incoming bytes:
  while (Serial.available() > 0)
  {
    b = Serial.read();
    Log.infoln("BEFORE: %d %d", parser.state, b);
    uint8_t change = pgm_read_byte(STATE_TABLE[parser.state - 1][b]);
    // Log.infoln("MIDDLE >>>> change: %d new_state: %d, action: %d", change, STATE(change), ACTION(change));
    do_state_change(&parser, change, b);
    Log.infoln("AFTER %d", parser.state);
  }

  memset(to_serial, 0, 3);
  read_kbd(to_serial);
  if (to_serial[0])
  {
    Serial.write(to_serial, strlen(to_serial));
  }
}