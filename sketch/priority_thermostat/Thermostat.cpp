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

#include "Thermostat.h"

#define UNDEF -9999

/*
 * Constructor
 */
Thermostat::Thermostat(int _pin) {
  pin = _pin;
  rawIndex = 0;
  temperature = UNDEF;
}

/*
 * Sample temperature and millis
 */
void Thermostat::sample() {
  sample(millis());
}

/*
 * Sample temperature
 */
void Thermostat::sample(unsigned long _millis) {
  raw[rawIndex++] = analogRead(pin);
  delay(10);

  if(rawIndex >= SAMPLE_SET_SIZE) {
    rawIndex = 0;
    temperature = 0; // anything other than UNDEF, actual temperature will be recalculated.
  }

  // Determine average of sample set
  int average = 0;
  for(int i=0; i<SAMPLE_SET_SIZE; ++i) {
    average += raw[i];
  }
  average /= SAMPLE_SET_SIZE;

  // Interpolate 
  
  
}

