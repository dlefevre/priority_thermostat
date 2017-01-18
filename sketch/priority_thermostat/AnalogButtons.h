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

#ifndef _ANALOGBUTTON_H_
#define _ANALOGBUTTON_H_

#define LONGPRESS_TIME 1000

#include <Arduino.h>

template<size_t N>
class AnalogButtons {
  public:
    AnalogButtons(int _pin, int _tolerance);
    void set(int _index, int _analogValue);
    void sample();
    void sample(unsigned long _millis);
    int getPressed();
    bool isShortPress();
    bool isLongPress();

  private:
    int pin;
    int tolerance;
    int lowValues[N];
    int highValues[N];
    int lastPressedButton;
    bool shortPress;
    bool longPress;
    unsigned long downTimestamp;
    
};

/*
 * Constructor, N defines the number of buttons on the analog pin.
 */
template<size_t N>
AnalogButtons<N>::AnalogButtons(int _pin, int _tolerance) {
  pin = _pin;
  tolerance = _tolerance;
  lastPressedButton = -1;
  shortPress = false;
  longPress = false;
}

/*
 * Define a button's analog value. 
 */
template<size_t N>
void AnalogButtons<N>::set(int _index, int _analogValue) {
  lowValues[_index] = _analogValue - tolerance;
  highValues[_index] = _analogValue + tolerance;
}

/*
 * Sample the analog line and millis
 */
template<size_t N>
void AnalogButtons<N>::sample() {
  return sample(millis());
}

/*
 * Sample the analog line and determine the pressed button.
 */
template<size_t N>
void AnalogButtons<N>::sample(unsigned long _millis) {
  int value = analogRead(pin);
  delay(10);

  bool pressed = false;
  for(int i=0; i<N; ++i) {
    if(value >= lowValues[i] && value <= highValues[i]) {
      // Detected short press
      if(i != lastPressedButton) {
        lastPressedButton = i;
        shortPress = true;
        downTimestamp = _millis;
      } else {
        shortPress = false;

        // Detect long press
        if(downTimestamp + LONGPRESS_TIME < _millis) {
          longPress = true;
        } else
          longPress = false;
      }

      pressed = true;
    } 
  }
      
  if(!pressed) {
    shortPress = false;
    longPress = false;
    lastPressedButton = -1;
  }
}

/*
 * Return the last pressed button
 */
template<size_t N>
int AnalogButtons<N>::getPressed() {
  return lastPressedButton;
}

/*
 * Returns true if sample() detected a short press
 */
template<size_t N>
bool AnalogButtons<N>::isShortPress() {
  return shortPress;
}

/*
 * Returns true if sample() detected a long press
 */
template<size_t N>
bool AnalogButtons<N>::isLongPress() {
  return longPress;
}

#endif

