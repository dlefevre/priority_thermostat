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

#define UNDEF -9999

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
 * Retrieve formatted temperature
 */
char * Thermostat::getTemperatureStr(char * _buffer) {
  char localBuffer[10];
  memset(localBuffer, '\0', 10);

  // Do the math
  int integer = temperature / 100;
  int decimal = temperature % 100;
  if(decimal < 25) {
    decimal = 0;
  } else if(decimal >= 25 && decimal < 50) {
    decimal = 5;
  } else if(decimal >= 50 && decimal < 75) {
    decimal = 5;
  } else {
    ++integer;
    decimal = 0;
  }

  // Integer part
  itoa(integer, localBuffer, 10);
  memcpy(_buffer, localBuffer, strlen(localBuffer) + 1);
  memcpy(&_buffer[strlen(_buffer)], ".", 2);
  itoa(decimal, localBuffer, 10);
  memcpy(&_buffer[strlen(_buffer)], localBuffer, 1);
  _buffer[strlen(_buffer)] = '\0';

  return _buffer;
}

/*
 * Retrieve formatted requested temperature
 */
char * Thermostat::getRequestedTemperatureStr(char * _buffer) {
  return _buffer;
}

/*
 * Retrieve formatted hysteresis value
 */
char * Thermostat::getHysteresisStr(char * _buffer) {
  return _buffer;
}

/*
 * Retrieve formatted maximum time the boiler is heated before going in alarm.
 */
char * Thermostat::getMaxHeatTime(char * _buffer) {
  return _buffer;
}

/*
 * Retrieve formatted maximum temperature, until going in alarm.
 */
char * Thermostat::getMaxTemperature(char * _buffer) {
  return _buffer;
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
void Thermostat::changeRequestedTemperature(int _delta) {
  requestedTemperature += _delta;
}

/*
 * Change hysteresis value
 */
void Thermostat::changeHysteresis(int _delta) {
  hysteresis += _delta;
}

/*
 * Change maximum heat time
 */
void Thermostat::changeMaxHeatTime(int _delta) {
  maximumHeatTime += _delta;
}

/*
 * Change maximum temperature
 */
void Thermostat::changeMaxTemperature(int _delta) {
  maximumTemperature += _delta;
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
  if(_value >= calX[SAMPLE_SET_SIZE - 1]) {
    i0 = SAMPLE_SET_SIZE - 2;
  } else {
    for(byte i=0; i<SAMPLE_SET_SIZE - 1; ++i) {
      if(_value >= calX[i]) {
        i0 = i;
        break;
      }
    }
  }

  // Interpolate
  const byte i1 = i0 + 1;
  const long xPart = (_value - calX[i0]) / (calX[i1] - calX[i0]);  
  temperature = calY[i0] * (1 - xPart) + calY[i1] * xPart;
}

