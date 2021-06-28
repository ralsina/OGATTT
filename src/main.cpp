#include <Arduino.h>
#include <ArduinoLog.h>
#include "terminal.h"
#include "kbd.h"
#include "screen.h"

Terminal term;
Keyboard kbd;
Screen screen;

void setup()
{
  // Initialize serial port
  Serial.begin(57600);
  while (!Serial && !Serial.available())
  {
  };
  Serial.setTimeout(50);

  // Initialize logging
  // Can log to &Serial or &term.oled

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  term.init(kbd, screen);
  Log.infoln("Terminal Ready: %dx%d\r", SCREEN_COLS, SCREEN_ROWS);

}

void loop()
{
  term.tick();
}