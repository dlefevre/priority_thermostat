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

#ifndef _THERMOSTAT_H_
#define _THERMOSTAT_H_

#define SAMPLE_SET_SIZE 10

#include <Arduino.h>

/*
 * Implements an on/off thermostat that uses a hystersis loop and 
 * linear interpollation on a calibration set to determine
 * the temperature. We get away with linear interpollation since 
 * our temperature curve is quite straight and I can live with a one
 * or two degree miss on my boiler temperature.
 * 
 * Raw number are multiplied by a factor 100 to prevent floating point
 * arithmetic.
 */
class Thermostat {
  public:
    Thermostat(byte _pin);
    void sample();
    void sample(unsigned long _millis);

    // Retrieve values (formatted for the LCD)
    char * getTemperatureStr(char *);
    char * getRequestedTemperatureStr(char *);
    char * getHysteresisStr(char *);
    char * getMaxHeatTime(char *);
    char * getMaxTemperature(char *);
    long getRaw();

    // Change values (based on some constants set in the main sketch 
    void changeRequestedTemperature(int);
    void changeHysteresis(int);
    void changeMaxHeatTime(int);
    void changeMaxTemperature(int);

  private:
    const long calX[5] = {43500, 47600, 51500, 55300, 58700}; // factor 100
    const long calY[5] = {1000, 3000, 5000, 7000, 9000};      // factor 100
    byte pin;
    long raw[SAMPLE_SET_SIZE]; // factor 100
    byte rawIndex;
    
    int temperature;
    int requestedTemperature;
    int hysteresis;
    unsigned long maximumHeatTime;
    int maximumTemperature;
    
    long calculateAverage();
    void interpolateTemperature(long _value);
    
};

#endif

