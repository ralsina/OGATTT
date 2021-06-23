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

void loop()
{
  // read the incoming bytes:
  while (Serial.available())
  {
    term.process(Serial.read());
  }

  term.read_kbd();
  if (term.kbd_buffer[0])
  {
    Serial.write(term.kbd_buffer, strlen(term.kbd_buffer));
  }
}