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
#include "stdlib.h"

/*
 * Constructor
 */
Thermostat::Thermostat(byte _pin) {
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
  // Pull value from our analog pin
  raw[rawIndex++] = (long)analogRead(pin) * 100L;
  delay(10);

  // Round robin
  if(rawIndex >= SAMPLE_SET_SIZE) {
    rawIndex = 0;
    temperature = 0; // anything other than UNDEF, actual temperature will be recalculated.
  }

  // Wait for the sample set to stabilize.
  if(temperature == UNDEF) {
    return;
  }

  // Determin the actual temperature
  long average = calculateAverage();
  interpolateTemperature(average); 
}

/*
 * Retrieve temperature
 */
int Thermostat::getTemperature() {
  return temperature;
}

/*
 * Retrieve requested temperature
 */
int Thermostat::getRequestedTemperature() {
  return requestedTemperature;
}

/*
 * Retrieve hysteresis value
 */
int Thermostat::getHysteresis() {
  return hysteresis;
}

/*
 * Retrieve maximum time the boiler is heated before going in alarm.
 */
unsigned long Thermostat::getMaxHeatTime() {
  return maximumHeatTime;
}

/*
 * Retrieve maximum temperature, until going in alarm.
 */
int Thermostat::getMaxTemperature() {
  return maximumTemperature;
}

/*
 * Retrieve minimum temperature, until going in alarm.
 */
int Thermostat::getMinTemperature() {
  return minimumTemperature;
}

/*
 * Retrieve raw value
 */
long Thermostat::getRaw() {
  return calculateAverage();
}

/*
 * Change the requested temperature
 */
void Thermostat::setRequestedTemperature(int _value) {
  requestedTemperature = _value;
}

/*
 * Change hysteresis value
 */
void Thermostat::setHysteresis(int _value) {
  hysteresis = _value;
}

/*
 * Change maximum heat time
 */
void Thermostat::setMaxHeatTime(int _value) {
  maximumHeatTime = _value;
}

/*
 * Change maximum temperature
 */
void Thermostat::setMaxTemperature(int _value) {
  maximumTemperature = _value;
}

/*
 * Determine average of the sample set
 */
long Thermostat::calculateAverage() {
  long average = 0;
  for(byte i=0; i<SAMPLE_SET_SIZE; ++i) {
    average += raw[i];
  }
  
  return average / SAMPLE_SET_SIZE;
}

/*
 * Interpolate the actual temperature
 */
void Thermostat::interpolateTemperature(long _value) {
  // Determine reference frame (when going out of bounds, take the closest)
  byte i0 = 0;
  if(_value < calX[0]) {
    i0 = 0;
  } else {
    for(byte i=CALIBRATION_SET_SIZE - 2; i>=0; --i) {
      if(_value >= calX[i]) {
        i0 = i;
        break;
      }
    }
  }

  // Interpolate.
  byte i1 = i0 + 1;
  long xPart = (_value - calX[i0]) * FIXEDPOINT_MLT1 / (calX[i1] - calX[i0]);
  long tmp = calY[i0] * (FIXEDPOINT_MLT1 - xPart) + calY[i1] * xPart;
  temperature = tmp / FIXEDPOINT_MLT1;
}



