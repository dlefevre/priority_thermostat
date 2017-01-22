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

/*
 * Helper function for formating and rouding temperatures
 */
char * formatTemperature(char * _buffer, long _temperature) {
  if(_temperature == UNDEF) {
    sprintf(_buffer, "-");
    return _buffer;
  }
  
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
  sprintf(_buffer, "%d.%d%cC", integer, decimal, (char)223);
  return _buffer;
}

/*
 * Helper function for formatting time (limited to minutes)
 */
char * formatTimeM(char * _buffer, unsigned long _time) {
  // Do the math
  _time /= 1000; // convert milliseconds to seconds
  int hours = _time / 3600;
  int minutes = (_time % 3600) / 60;

  // Format
  sprintf(_buffer, "%dh%02d'", hours, minutes);
  return _buffer;
}

/*
 * Helper function for formatting time (limited to seconds)
 */
char * formatTimeS(char * _buffer, unsigned long _time) {
  // Do the math
  _time /= 1000; // convert milliseconds to seconds
  int hours = _time / 3600;
  int rest = _time % 3600;
  int minutes = rest / 60;
  int seconds = rest % 60;

  // Format
  sprintf(_buffer, "%dh%02d'%02d\"", hours, minutes, seconds);
  return _buffer;
}

/*
 * Helper function for getting a pointer to the end of a string
 */
char * appendPtr(char * _str) {
  return &_str[strlen(_str)];
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
  menuPosition = 0;
  resetMode = RESET_NO;
  loadParameters();
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
 * Return the selected reset mode
 */
int Interface::getResetMode() {
  return inSetMode ? RESET_NO : resetMode;
}

/*
 * Load all thermostat parameters (thermostat -> interface)
 */
void Interface::loadParameters() {
  if(inMenu) {
    hysteresis = thermostat->getHysteresis();
    minimumTemperature = thermostat->getMinTemperature();
    maximumTemperature = thermostat->getMaxTemperature();
    maximumHeatTime = thermostat->getMaxHeatTime();
    graceTime = thermostat->getGraceTime();
    offsetTemperature = thermostat->getOffsetTemperature();
  } else {
    requestedTemperature = thermostat->getRequestedTemperature();
  }
}

/*
 * Save all parameters (interface -> thermostat)
 */
void Interface::saveParameters() {
  if(inMenu) {
    thermostat->setHysteresis(hysteresis);
    thermostat->setMinTemperature(minimumTemperature);
    thermostat->setMaxTemperature(maximumTemperature);
    thermostat->setMaxHeatTime(maximumHeatTime);
    thermostat->setGraceTime(graceTime);
    thermostat->setOffsetTemperature(offsetTemperature);
  } else {
    thermostat->setRequestedTemperature(requestedTemperature);
  }

  thermostat->save();
}

/*
 * Process an inrement for a parameter
 */
void Interface::processParameterIncrement(int _multiplier) {
  if(inMenu) {
    switch(menuPosition) {
      case 0:
        hysteresis += INCR_HYSTERESIS * _multiplier;
        break;
      case 1:
        minimumTemperature += INCR_MIN_TEMPERATURE * _multiplier;
        break;
      case 2:
        maximumTemperature += INCR_MAX_TEMPERATURE * _multiplier;
        break;
      case 3:
        maximumHeatTime += INCR_MAX_HEAT_TIME * _multiplier;
        break;
      case 4:
        graceTime += INCR_GRACE_TIME * _multiplier;
        break;
      case 5:
        offsetTemperature += INCR_OFFSET_TEMPERATURE * _multiplier;
        break;
      case 6:
        resetMode = (resetMode + _multiplier) % 3;
        if(resetMode < 0) {
          resetMode = 2;
        }
        break;
    }
  } else {
    requestedTemperature += INCR_REQUESTED_TEMPERATURE * _multiplier;
  }
}

/*
 * Manage interaction on the status screen
 */
void Interface::interactStatusScreen(unsigned long _millis) {
  if(buttons->isLongPress()) {
    if(buttons->getPressed() == BUTTON_MENU) {
      inMenu = true;
      inSetMode = false;
      menuPosition = 0;
      loadParameters();
    } else if(buttons->getPressed() == BUTTON_SET && !inSetMode) {
      inSetMode = true;
      loadParameters();
    }
  }

  if(buttons->isShortPress()) {
    if(buttons->getPressed() == BUTTON_SET && inSetMode) {
      inSetMode = false;
      saveParameters();
    } else if(buttons->getPressed() == BUTTON_INCREASE && inSetMode) {
      processParameterIncrement(-1);
    } else if(buttons->getPressed() == BUTTON_DECREASE && inSetMode) {
      processParameterIncrement(1);
    }
  }
}

/*
 * Manage interaction on the menu screen
 */
void Interface::interactMenuScreen(unsigned long _millis) {
  if(buttons->isShortPress()) {
    // Menu navigation
    if(buttons->getPressed() == BUTTON_MENU) {
      
      inMenu = false;
      inSetMode = false;
      loadParameters();
      
   } else if(buttons->getPressed() == BUTTON_INCREASE) {
    
      if(inSetMode) {
        processParameterIncrement(1);
      } else {
        menuPosition = (menuPosition + 1) % NUMBER_MENU_ITEMS;
      }
      
    } else if(buttons->getPressed() == BUTTON_DECREASE) {
      
      if(inSetMode) {
        processParameterIncrement(-1);
      } else {
        if(menuPosition == 0) {
          menuPosition = NUMBER_MENU_ITEMS - 1;
        } else {
          --menuPosition;
        }
      }
      
    } else if(buttons->getPressed() == BUTTON_SET && inSetMode) {
      inSetMode = false;
      saveParameters();
    }
  }
  
  if(buttons->isLongPress() && buttons->getPressed() == BUTTON_SET && !inSetMode) {
    inSetMode = true;
  }
}

/*
 * Render the status screen
 */
void Interface::renderStatusScreen(unsigned long _millis) {
  clearBuffer();
  
  strcpy(buffer[0], "Cur.:  ");
  formatTemperature(appendPtr(buffer[0]), thermostat->getTemperature());
  strcpy(buffer[1], "Req.:  ");
  formatTemperature(appendPtr(buffer[1]), requestedTemperature);
  if(inSetMode) {
    strcpy(&buffer[1][15], "(set)");
  }
  strcpy(buffer[2], "Stat.: ");
  if(resetMode != RESET_NO) {
    strcpy(appendPtr(buffer[2]), "resetting");
  } else {
    strcpy(appendPtr(buffer[2]), thermostat->getStatus());
  }
  strcpy(buffer[3], "Time:  ");
  formatTimeS(appendPtr(buffer[3]), thermostat->getTimeSinceStatusChange());
  
  writeToLcd();
}

/*
 * Render the menu screen
 */
void Interface::renderMenuScreen(unsigned long _millis) {
  clearBuffer();

  // Populate the menu
  byte menuScreen = menuPosition / 3;
  if(menuScreen == 0) {
    strcpy(buffer[0], "---- MENU (1/3) ----");
    strcpy(buffer[1], " Hyst.:     ");
    formatTemperature(appendPtr(buffer[1]), hysteresis);
    strcpy(buffer[2], " Min. tmp.: ");
    formatTemperature(appendPtr(buffer[2]), minimumTemperature);
    strcpy(buffer[3], " Max. tmp.: ");
    formatTemperature(appendPtr(buffer[3]), maximumTemperature);
  } else if (menuScreen == 1) {
    strcpy(buffer[0], "---- MENU (2/3) ----");
    strcpy(buffer[1], " Max. heat: ");
    formatTimeM(appendPtr(buffer[1]), maximumHeatTime);
    strcpy(buffer[2], " Grace tm.: ");
    formatTimeM(appendPtr(buffer[2]), graceTime);
    strcpy(buffer[3], " Offset:    ");
    formatTemperature(appendPtr(buffer[3]), offsetTemperature);
  } else if (menuScreen == 2) {
    strcpy(buffer[0], "---- MENU (3/3) ----");
    strcpy(buffer[1], " Reset md.: ");
    switch(resetMode) {
      case RESET_NO:
        strcpy(appendPtr(buffer[1]), "no");
        break;
      case RESET_NORMAL:
        strcpy(appendPtr(buffer[1]), "normal");
        break;
      case RESET_FACTORY:
        strcpy(appendPtr(buffer[1]), "factory");
        break;
      default:
        strcpy(appendPtr(buffer[1]), "error");
        break;
    }
  } else {
    sprintf(buffer[0], "# ERROR #");
  }

  // Set the cursor
  buffer[(menuPosition % 3) + 1][0] = inSetMode ? '*' : '>';

  writeToLcd();
}

/*
 * Clear the buffer for the LCD
 */
void Interface::clearBuffer() {
  for(int i=0; i<LCD_ROWS; ++i) {
    for(int j=0; j<LCD_COLUMNS; ++j) {
      buffer[i][j] = ' ';
    }
  } 
}

/*
 * Dump the buffer to the LCD. '\0' characters in the
 * middle of a line are replaced by space.
 */
void Interface::writeToLcd() {
  for(int i=0; i<LCD_ROWS; ++i) {
    for(int j=0; j<LCD_COLUMNS; ++j) {
      if(buffer[i][j] == '\0') {
        buffer[i][j] = ' ';
      }
    }
    buffer[i][LCD_COLUMNS] = '\0';

    lcd->setCursor(0, i);
    char * ptr = buffer[i];
    lcd->print(ptr);
  }
}

