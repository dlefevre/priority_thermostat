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

#ifndef _MAGICNUMBERS_H_
#define _MATICNUMBERS_H_

// LCD pins
#define LCD_RS_PIN     2
#define LCD_ENABLE_PIN 3
#define LCD_D4_PIN     4
#define LCD_D5_PIN     5
#define LCD_D6_PIN     6
#define LCD_D7_PIN     7
#define LCD_LED_PIN    8

// LCD dimensions
#define LCD_ROWS    4
#define LCD_COLUMNS 20

// Other pins
#define THERMISTOR_PIN A0
#define BUTTONS_PIN    A1
#define RELAY_PIN      9
#define ENABLE_PIN     10

// Tolerance on the analog value for the buttons.
#define ANALOG_TOLERANCE 10

// How long a button should be held down before we consider it
// a long-press.
#define LONGPRESS_TIME 1000

// Button numbers
#define NUMBER_OF_BUTTONS 4
#define BUTTON_INCREASE 0
#define BUTTON_DECREASE 1
#define BUTTON_SET      2
#define BUTTON_MENU     3

// Some magic variables for the thermostat logic
#define UNDEF           -9999
#define FIXEDPOINT_MLT1 1000L

// Sizes for the arrays
#define SAMPLE_SET_SIZE 10
#define CALIBRATION_SET_SIZE 5

// Defaults
#define DEFAULT_REQUESTED_TEMPERATURE 5000
#define DEFAULT_HYSTERESIS            500
#define DEFAULT_MIN_TEMPERATURE       0
#define DEFAULT_MAX_TEMPERATURE       8500
#define DEFAULT_MAX_HEAT_TIME         7200000L
#define DEFAULT_GRACE_TIME            120000L
#define DEFAULT_OFFSET_TEMPERATURE    0
#define INCR_REQUESTED_TEMPERATURE 50
#define INCR_HYSTERESIS            50
#define INCR_MIN_TEMPERATURE       100
#define INCR_MAX_TEMPERATURE       100
#define INCR_MAX_HEAT_TIME         60000L 
#define INCR_GRACE_TIME            60000L 
#define INCR_OFFSET_TEMPERATURE    50

// Number of menu items
#define NUMBER_MENU_ITEMS 7

// Reset modes
#define RESET_NO      0
#define RESET_NORMAL  1
#define RESET_FACTORY 2

// EEPROM
#define EEPROM_TAG     {'P', 'T'}
#define EEPROM_VERSION 1

// Status
#define STATUS_STR         {"ready", "heating", "disabled", "grace period" }
#define STATUS_READY       0
#define STATUS_HEATING     1
#define STATUS_DISABLED    2
#define STATUS_GRACEPERIOD 3

// LCD backlight timeout (in ms.)
#define LCD_LED_TIMEOUT    120000

#endif

