#include <ArduinoLog.h>
#include <Arduino.h>
#include "terminal.h"
#include "kbd.h"

Terminal term;
Keyboard kbd;
#ifndef NATIVE
#include "ssd1306_screen.h"
SSD1306Screen screen;
#else
#include "screen.h"
Screen screen;
#endif

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