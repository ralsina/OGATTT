#include <Arduino.h>
#include "SSD1306Ascii.h"
#include <ArduinoLog.h>
#include "handlers.h"
#include "const.h"
#include "vtparse_table.hpp"
#include "terminal.h"

Terminal term;

void setup()
{
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial && !Serial.available())
  {
  };
  Serial.setTimeout(50);


  // Initialize logging
  // Can log to &Serial or &term.oled
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
}

uint8_t b;
char to_serial[3];

void loop()
{
  // read the incoming bytes:
  while (Serial.available())
  {
    term.process(Serial.read());
  }

  memset(to_serial, 0, 3);
  read_kbd(to_serial);
  if (to_serial[0])
  {
    Serial.write(to_serial, strlen(to_serial));
  }
}