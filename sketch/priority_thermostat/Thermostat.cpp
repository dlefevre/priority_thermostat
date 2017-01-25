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
#include <EEPROM.h>

typedef union ul_convert {
  unsigned long value;
  byte raw[4];
} ul_convert_t;

/*
 * Helper function for calculating the difference between two unsigned longs
 * while taking a wrap around into account.
 */
unsigned long diffUL(unsigned long _low, unsigned long _high) {
  if(_low > _high) {
    _high += 4294967295L - _low + 1L;
    _low = 0;
  }
  return _high - _low;
}

/*
 * Constructor
 */
Thermostat::Thermostat(byte _pinThermistor, byte _pinEnable) {
  pinThermistor = _pinThermistor;
  pinEnable = _pinEnable;
  rawIndex = 0;
  temperature = UNDEF;

  loadParameters();
  strcpy(status, "initializing");
  
  heating = false;
  enabled = false;
  inGracePeriod = true;
  lastHeatStart = 0;
  lastHeat = 0;
  lastStatusChange = 0;
  lastSerialOutput = 0;
  memset(status, '\0', 14);
  statusid = 0;
  alarm = false;
  serialEnabled = false;
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
  if(alarm) {
    return;
  }
  
  // Pull value from our analog pin
  raw[rawIndex++] = (long)analogRead(pinThermistor) * 100L;
  delay(10);

  // Round robin
  if(rawIndex >= SAMPLE_SET_SIZE) {
    rawIndex = 0;
    if(temperature == UNDEF) {
      temperature = 0;
    }
  }

  if(temperature == UNDEF) {
    return;
  }
  
  // Determine the actual temperature
  long average = calculateAverage();
  interpolateTemperature(average); 
  temperature += offsetTemperature;

  // Check if hot water is enabled by the heatlink (Nest).
  enabled = (digitalRead(ENABLE_PIN) == HIGH);
  inGracePeriod = diffUL(lastHeat, _millis) <= graceTime;

  // Boiler heating
  int halfRange = hysteresis / 2;
  if(!heating && temperature < requestedTemperature - halfRange) {
    heating = true; 
    lastHeatStart = _millis;
  }
  if(heating && temperature > requestedTemperature + halfRange) {
    heating = false;
    if(!inGracePeriod) {
      lastHeat = _millis;
    }
  }

  // Alarms
  if(temperature < minimumTemperature) {
    alarm = true;
    heating = false;
    sprintf(status, "alarm (min %c)", (char)223);
  } else if(temperature > maximumTemperature) {
    alarm = true;
    heating = false;
    sprintf(status, "alarm (max %c)", (char)223);
  }
  if(heating && diffUL(lastHeatStart, _millis) > maximumHeatTime) {
    alarm = true;
    heating = false;
    sprintf(status, "alarm (max t)");
  }

  // Reporting on serial console
  if(serialEnabled) {
    if(_millis - lastSerialOutput >= SERIAL_FREQUENCY) {
      Serial.print(_millis / 1000);
      Serial.print(";");
      Serial.print(temperature / 100); 
      Serial.print("."); 
      Serial.print(temperature % 100);
      Serial.print(";");
      Serial.print(requestedTemperature / 100); 
      Serial.print("."); 
      Serial.print(requestedTemperature % 100);
      Serial.print(";");
      Serial.print(hysteresis / 100);
      Serial.print("."); 
      Serial.print(hysteresis % 100);
      Serial.print(";");
      Serial.print(heating);
      Serial.print(";");
      Serial.print(enabled);
      Serial.print(";");
      Serial.print(inGracePeriod);
      Serial.print(";");
      Serial.print(lastHeatStart / 1000);
      Serial.print(";");
      Serial.print(lastHeat / 1000);
      Serial.print(";");
      Serial.print(lastStatusChange / 1000);
      Serial.print(";");
      Serial.println(alarm);

      lastSerialOutput = _millis;
    }
  }

  // Prevent any further status changes if an alarm was set
  if(alarm) {
    return;
  }

  // Report status
  byte previousStatusid = statusid;
  if(enabled && heating) {
    if(inGracePeriod) {
      statusid = STATUS_GRACEPERIOD;
    } else {
      statusid = STATUS_HEATING;
    }
  } else if(enabled && !heating) {
    statusid = STATUS_READY;
  } else if (!enabled) {
    statusid = STATUS_DISABLED;
  }
  strcpy(status, statusPrompts[statusid]);
  if(previousStatusid != statusid) {
    lastStatusChange = _millis;
  }
}

/*
 * Retrieve temperature
 */
int Thermostat::getTemperature() {
  return temperature;
}

/*
 * Check the heat condition
 */
bool Thermostat::shouldHeat() {
  return heating && enabled && !inGracePeriod && !alarm;
}

/*
 * Check if the system has generated an alarm
 */
bool Thermostat::inAlarm() {
  return alarm;
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
 * Retrieve grace time (time to wait before reengaging the heater).
 */
unsigned long Thermostat::getGraceTime() {
  return graceTime;
}

/*
 * Retrieve the temperature offset
 */
int Thermostat::getOffsetTemperature() {
  return offsetTemperature;
}

/*
 * Retrieve the thermostat's status
 */
char * Thermostat::getStatus() {
  return status;
}

/*
 * Return the time since the last status change
 */
unsigned long Thermostat::getTimeSinceStatusChange() {
  return diffUL(lastStatusChange, millis());
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
void Thermostat::setMaxHeatTime(unsigned long _value) {
  maximumHeatTime = _value;
}

/*
 * Change maximum temperature
 */
void Thermostat::setMaxTemperature(int _value) {
  maximumTemperature = _value;
}

/*
 * Change minimum temperature
 */
void Thermostat::setMinTemperature(int _value) {
  minimumTemperature = _value;
}

/*
 * Change the grace time.
 */
void Thermostat::setGraceTime(unsigned long _value) {
  graceTime = _value;
}

/*
 * Set the offset temperature
 */
void Thermostat::setOffsetTemperature(int _value) {
  offsetTemperature = _value;
}

/*
 * Enabled/Disable the serial console
 */
void Thermostat::setSerialEnabled(bool _value) {
  serialEnabled = _value;
  if(serialEnabled) {
    Serial.begin(9600);
  } else {
    Serial.end();
  }
}

/*
 * Retrieve the status of the serial console.
 */
bool Thermostat::getSerialEnabled() {
  return serialEnabled;
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

/*
 * Expose the save functionality (required for Interface).
 */
void Thermostat::save() {
  saveParameters();
}

/*
 * Clear out the EEPROM
 */
void Thermostat::factoryReset() {
  for (int i=0; i<EEPROM.length(); ++i) {
    EEPROM.update(i, 0);
  }
}

/*
 * Save all parameters
 */
void Thermostat::saveParameters() {
  byte tag[2] = EEPROM_TAG;
  byte version = EEPROM_VERSION;
  
  EEPROM.put(0, tag);
  EEPROM.put(2, version);
  EEPROM.put(3, requestedTemperature);
  EEPROM.put(5, offsetTemperature);
  EEPROM.put(7, hysteresis);
  EEPROM.put(9, maximumHeatTime);
  EEPROM.put(13, maximumTemperature);
  EEPROM.put(15, minimumTemperature);
  EEPROM.put(17, graceTime);
}

/*
 * Load all parameters
 */
void Thermostat::loadParameters() {
  byte tag[2];
  byte expectedTag[2] = EEPROM_TAG;
  byte version;

  // Verify tag
  EEPROM.get(0, tag);
  EEPROM.get(2, version);
  
  if(memcmp(tag, expectedTag, 2) != 0 || version != EEPROM_VERSION) {
    requestedTemperature = DEFAULT_REQUESTED_TEMPERATURE;
    hysteresis = DEFAULT_HYSTERESIS;
    minimumTemperature = DEFAULT_MIN_TEMPERATURE;
    maximumTemperature = DEFAULT_MAX_TEMPERATURE;
    maximumHeatTime = DEFAULT_MAX_HEAT_TIME;
    offsetTemperature = DEFAULT_OFFSET_TEMPERATURE;
    graceTime = DEFAULT_GRACE_TIME;
    
    saveParameters();
    return;
  }

  EEPROM.get(3, requestedTemperature);
  EEPROM.get(5, offsetTemperature);
  EEPROM.get(7, hysteresis);
  EEPROM.get(9, maximumHeatTime);
  EEPROM.get(13, maximumTemperature);
  EEPROM.get(15, minimumTemperature);
  EEPROM.get(17, graceTime);
}

