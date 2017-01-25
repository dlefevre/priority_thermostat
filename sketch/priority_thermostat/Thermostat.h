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

#include <Arduino.h>
#include "MagicNumbers.h"

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
    Thermostat(byte _pinThermistor, byte _pinEnables);
    void sample();
    void sample(unsigned long _millis);

    // Retrieve values
    int getRequestedTemperature();
    int getHysteresis();
    unsigned long getMaxHeatTime();
    int getMaxTemperature();
    int getMinTemperature();
    unsigned long getGraceTime();
    int getOffsetTemperature();
    bool getSerialEnabled();
    
    int getTemperature();
    bool shouldHeat();
    char * getStatus();
    unsigned long getTimeSinceStatusChange();
    bool inAlarm();
    bool serialEnabled;

    // Change values (based on some constants set in the main sketch 
    void setRequestedTemperature(int);
    void setHysteresis(int);
    void setMaxHeatTime(unsigned long);
    void setMaxTemperature(int);
    void setMinTemperature(int);
    void setGraceTime(unsigned long);
    void setOffsetTemperature(int);
    void setSerialEnabled(bool);

    void save();
    void factoryReset();

  private:
    const long calX[CALIBRATION_SET_SIZE] = {43500L, 47600L, 51500L, 55300L, 58700L};
    const long calY[CALIBRATION_SET_SIZE] = {1000L, 3000L, 5000L, 7000L, 9000L};
    const char statusPrompts[4][14] = STATUS_STR;

    // the mojo
    byte pinThermistor;
    byte pinEnable;
    long raw[SAMPLE_SET_SIZE];
    byte rawIndex;

    // the values
    int temperature;          // An integer is just about enough for my setup.
    int requestedTemperature;
    int offsetTemperature;
    int hysteresis;
    unsigned long maximumHeatTime;
    int maximumTemperature;
    int minimumTemperature;
    unsigned long graceTime;

    // the state
    bool heating;
    bool enabled;
    bool inGracePeriod;
    unsigned long lastHeatStart;
    unsigned long lastHeat;
    unsigned long lastStatusChange;
    unsigned long lastSerialOutput;
    char status[14];
    byte statusid; // use this so we don't have to compare strings all the time.
    bool alarm;
    
    long calculateAverage();
    void interpolateTemperature(long _value);
    void saveParameters();
    void loadParameters();
};

#endif

