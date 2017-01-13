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
 * Test sketch for determining the noise on the 3.3V channel and thermistor. 
 * The LCD display will (16x2) will display the minimum, maximum, delta and
 * average value.
 */
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define THERMISTORPIN A0

int minShown = 9999;
int maxShown = 0;
int avgShown = 0;
int delta = 0;
int minCollect = 1023;
int maxCollect = 0;
int count = 0;
int showCount = 0;
int lastShowCount = 0;

void setup() {
  lcd.begin(16, 2);
  analogReference(EXTERNAL);
  lcd.print("-");
  lcd.setCursor(0,1);
  lcd.print("-");
}

void loop() {
  int raw = analogRead(THERMISTORPIN);

  if(minCollect > raw) {
    minCollect = raw;
  }
  if(maxCollect < raw) {
    maxCollect = raw;
  }

  if(++count % 100 == 0) {
    avgShown = (minCollect + maxCollect) / 2;
    delta = maxCollect - minCollect;
    minShown = minCollect;
    maxShown = maxCollect;

    minCollect = 1023;
    maxCollect = 0;
    ++showCount;
    count = 0;
  }

  if(showCount != lastShowCount) {
    lcd.clear();
    lcd.print(minShown);
    lcd.print(" - ");
    lcd.print(maxShown);
    lcd.setCursor(12,0);
    lcd.print(showCount, HEX);
    lcd.setCursor(0, 1);
    lcd.print(avgShown);
    lcd.setCursor(7, 1);
    lcd.print("d: ");
    lcd.print(delta);

    lastShowCount = showCount;
  }
  
  delay(15);
}
