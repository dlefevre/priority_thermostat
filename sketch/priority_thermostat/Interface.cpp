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

#include "Interface.h"
#include <stdlib.h>

// Global buffer for formatters
char formattingBuffer[11];

/*
 * Helper function for formatting temperatures. This function
 * uses the global formatting buffer which can be overwritten
 * at any point in time.
 */
char * formatTemperature(long _temperature) {
  formatTemperature(formatBuffer, _temperature);
}

/*
 * Helper function for formating and rouding temperatures
 */
char * formatTemperature(char * _buffer, long _temperature) {
  // Do the math
  int integer = _temperature / 100;
  int decimal = _temperature % 100;
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

  // Format
  sprintf(_buffer, "%d.%d C", integer, decimal);
  return _buffer;
}

/*
 * Helper function for formatting time. This function
 * uses the global formatting buffer which can be overwritten
 * at any point in time.
 */
char * formatTime(unsigned long _time) {
  formatTime(formatBuffer, _time);
}

/*
 * Helper function for formatting time
 */
char * formatTime(char * _buffer, unsigned long _time) {
  // Do the math
  _time /= 1000; // convert milliseconds to seconds
  int hours = _time / 3600;
  int remainder = _time % 3600;
  int minutes = _time / 60;
  int seconds = _time % 60;

  // Format
  sprintf(_buffer, "%dh%02d'%02d\"", hours, minutes, seconds);
  return _buffer;
}

/*
 * Constructor
 */
Interface::Interface(LiquidCrystal * _lcd, 
                     AnalogButtons<NUMBER_OF_BUTTONS> * _buttons,
                     Thermostat * _thermostat) {
  lcd = _lcd;
  buttons = _buttons;
  thermostat = _thermostat;

  inMenu = false;
}

/*
 * Manage interaction (menu's and thermostat)
 */
void Interface::interact() {
  interact(millis());
}
/*
 * Manage interaction (menu's and thermostat)
 */
void Interface::interact(unsigned long _millis) {
  if(inMenu) {
    interactMenuScreen(_millis);
  } else {
    interactStatusScreen(_millis);
  }
}

/*
 * Render content on LCD.
 */
void Interface::render() {
  render(millis());
}

/*
 * Render content on LCD.
 */
void Interface::render(unsigned long _millis) {
   if(inMenu) {
    renderMenuScreen(_millis);
  } else {
    renderStatusScreen(_millis);
  }
}

/*
 * Manage interaction on the status screen
 */
void Interface::interactStatusScreen(unsigned long _millis) {
  
}

/*
 * Manage interaction on the menu screen
 */
void Interface::interactMenuScreen(unsigned long _millis) {
  
}

/*
 * Render the status screen
 */
void Interface::renderStatusScreen(unsigned long _millis) {
  
}

/*
 * Render the menu screen
 */
void Interface::renderMenuScreen(unsigned long _millis) {
  
}

