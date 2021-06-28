#include <ArduinoLog.h>
#include <Arduino.h>
#include "terminal.h"
#include "kbd.h"
#include "ssd1306_screen.h"

Keyboard kbd;
SSD1306Screen screen;
Terminal term(&kbd, &screen);

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
  Log.infoln("Initializing terminal.\r");
  term.init();
  Log.infoln("Terminal Ready: %dx%d\r", SCREEN_COLS, SCREEN_ROWS);

}

void loop()
{
  term.tick();
}