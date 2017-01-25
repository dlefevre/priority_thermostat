/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org>
 */

/*
 * Description: 
 *   This sketch implements a priority thermostat:
 *    - The thermostat determines the boiler temperature using a thermistor. Since
 *      I'm lacking specs for the thermistor that I "recycled" I interpolate the temperature
 *      from a set of calibration values.
 *    - The boiler's relays are activated/deactivated using a simple hysteresis loop. 
 *    - There's a basic interface through 4 buttons and a 20x4 lcd display.
 * Author: Dusty Lefevre
 */

#include <LiquidCrystal.h>
#include "AnalogButtons.h"
#include "Thermostat.h"
#include "MagicNumbers.h"
#include "Interface.h"

// Objects required for our used features
LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, 
                  LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
AnalogButtons<NUMBER_OF_BUTTONS> buttons(BUTTONS_PIN, ANALOG_TOLERANCE);
Thermostat thermostat(THERMISTOR_PIN, ENABLE_PIN);
Interface interface(&lcd, &buttons, &thermostat);

void setup() {
  // Set up LCD
  pinMode(LCD_RS_PIN, OUTPUT);
  pinMode(LCD_ENABLE_PIN, OUTPUT);
  pinMode(LCD_D4_PIN, OUTPUT);
  pinMode(LCD_D5_PIN, OUTPUT);
  pinMode(LCD_D6_PIN, OUTPUT);
  pinMode(LCD_D7_PIN, OUTPUT);
  pinMode(LCD_LED_PIN, OUTPUT);

  digitalWrite(LCD_LED_PIN, HIGH);
  lcd.begin(20, 4);

  // Set up relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  // Set up 3.3V reference
  analogReference(EXTERNAL);

  // Enable pin
  pinMode(ENABLE_PIN, INPUT);

  // Add buttons
  buttons.set(BUTTON_DECREASE, 1020);
  buttons.set(BUTTON_INCREASE, 926);
  buttons.set(BUTTON_SET, 690);
  buttons.set(BUTTON_MENU, 506);
}

void loop() {  
  buttons.sample();
  thermostat.sample();
  interface.interact();
  interface.render();

  // Activate/deactivate the heater
  if(thermostat.shouldHeat()) {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Activate/deactivate the LCD backlight
  if(thermostat.inAlarm() || buttons.recentlyActive()) {
    digitalWrite(LCD_LED_PIN, HIGH);
  } else {
    digitalWrite(LCD_LED_PIN, LOW);
  }

  // Check if we have to reset our board
  int resetMode = interface.getResetMode();
  if(resetMode != RESET_NO) {
    if(resetMode == RESET_FACTORY) {
      thermostat.factoryReset();
    }
    asm volatile ("jmp 0");
  }
  
  delay(80);
}
