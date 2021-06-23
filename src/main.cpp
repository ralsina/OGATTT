#include <Arduino.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <avr/pgmspace.h>
#include <ArduinoLog.h>
#include "handlers.h"
#include "const.h"
#include "vtparse_table.hpp"

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
  // Can log to &Serial or &oled
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.infoln("LOGGING GOES HERE\r\r");
  init_terminal();
  Log.infoln("INITIAL: %s\r", STATE_NAMES[parser.state]);
}

uint8_t b;
char to_serial[3];

void loop()
{
  // read the incoming bytes:
  while (Serial.available() > 0)
  {
    b = Serial.read();
    Log.infoln("BEFORE: %s %d\r", STATE_NAMES[parser.state], b);
    uint8_t change = pgm_read_byte(STATE_TABLE + (parser.state - 1) * 160 + b);
    Log.infoln(
        "MIDDLE: new_state: %s, action: %s\r",
        STATE_NAMES[STATE(change)], ACTION_NAMES[ACTION(change)]);
    do_state_change(&parser, change, b);
    Log.infoln("AFTER %s\r", STATE_NAMES[parser.state]);
  }

  memset(to_serial, 0, 3);
  read_kbd(to_serial);
  if (to_serial[0])
  {
    Serial.write(to_serial, strlen(to_serial));
  }
  delay(1000);
}