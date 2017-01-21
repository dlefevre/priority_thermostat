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

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "MagicNumbers.h"
#include "Thermostat.h"
#include "AnalogButtons.h"

/*
 * Implements and interface with:
 *  - A status screen, displaying:
 *    - current temperature
 *    - requested temperature (the requested temperature can be changed)
 *    - status
 *  - A menu, allowing us to change:
 *    - 0: the hysteresis value
 *    - 1: minimum temperature (alarms)
 *    - 2: maximum temperature (alarms)
 *    - 3: maximum run time (alarms)
 */
class Interface {
  public:
    Interface(LiquidCrystal *, AnalogButtons<NUMBER_OF_BUTTONS> *, Thermostat *);
    void interact();
    void interact(unsigned long _millis);
    void render();
    void render(unsigned long _millis);

  private:
    LiquidCrystal * lcd;
    AnalogButtons<NUMBER_OF_BUTTONS> * buttons;
    Thermostat * thermostat;

    char buffer[LCD_ROWS][LCD_COLUMNS + 1];
    byte padding[16]; //just in case
    
    bool inSetMode;
    bool inMenu;
    byte menuPosition;

    int requestedTemperature;
    int hysteresis;
    int minimumTemperature;
    int maximumTemperature;
    unsigned long maximumHeatTime;

    void loadParameters();
    void saveParameters();
    void processParameterIncrement(int);

    void interactStatusScreen(unsigned long _millis);
    void interactMenuScreen(unsigned long _millis);
    void renderStatusScreen(unsigned long _millis);
    void renderMenuScreen(unsigned long _millis);

    void clearBuffer();
    void writeToLcd();
};

#endif

