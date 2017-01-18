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

// LCD pins
#define LCD_RS_PIN 2
#define LCD_ENABLE_PIN 3
#define LCD_D4_PIN 4
#define LCD_D5_PIN 5
#define LCD_D6_PIN 6
#define LCD_D7_PIN 7
#define LCD_LED_PIN 8

// Other pins
#define THERMISTOR_PIN A0
#define BUTTONS_PIN A1
#define RELAY_PIN 9
#define ON_OFF_PIN 10

// Other magic variables
#define ANALOG_TOLERANCE 10

LiquidCrystal lcd(LCD_RS_PIN,
                  LCD_ENABLE_PIN,
                  LCD_D4_PIN,
                  LCD_D5_PIN,
                  LCD_D6_PIN,
                  LCD_D7_PIN);
AnalogButtons<4> buttons(BUTTONS_PIN, ANALOG_TOLERANCE);
Thermostat thermostat(THERMISTOR_PIN);

int count = 0;
int sampleBuffer[10];
int sampleIdx = 0;
int lastAverage = 0;
int lastButtonValue = 0;

void setup() {
  Serial.begin(9600);
  
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

  // Add buttons
  buttons.set(0, 1020);
  buttons.set(1, 926);
  buttons.set(2, 688);
  buttons.set(3, 501);

  // Initialize average
  for(int i=0; i<10; ++i) {
    sampleBuffer[i] = -1;
  }
}

void loop() {
  int thermistorValue = analogRead(THERMISTOR_PIN);
  delay(10);
  
  sampleBuffer[sampleIdx] = thermistorValue;
  sampleIdx = (sampleIdx + 1) % 10;

  // Determine average
  int average = 0;
  for(int i=0; i<10; ++i) {
    average += sampleBuffer[i];
  }
  average /= 10;

  buttons.sample();
  if(buttons.isShortPress()) {
    Serial.print("short: ");
    Serial.println(buttons.getPressed());
  }
  if(buttons.isLongPress()) {
    Serial.print("long: ");
    Serial.println(buttons.getPressed());
  }

  delay(100);
}
