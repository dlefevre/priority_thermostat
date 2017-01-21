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

// Other pins
#define THERMISTOR_PIN A0
#define BUTTONS_PIN    A1
#define RELAY_PIN      9
#define ON_OFF_PIN     10

// Tolerance on the analog value for the buttons.
#define ANALOG_TOLERANCE 10

// How long a button should be held down before we consider it
// a long-press.
#define LONGPRESS_TIME 1000

// Button numbers
#define NUMBER_OF_BUTTONS 4
#define BUTTON_DECREASE 0
#define BUTTON_INCREASE 1
#define BUTTON_SET      2
#define BUTTON_MENU     3

// Some magic variables for the thermostat logic
#define UNDEF           -9999
#define FIXEDPOINT_MLT1 1000L

// Sizes for the arrays
#define SAMPLE_SET_SIZE 10
#define CALIBRATION_SET_SIZE 5

// Defaults
#define DEFAULT_REQUESTED_TEMPERATURE 50
#define DEFAULT_HYSTERESIS 5
#define DEFAULT_MAX_TEMPERATURE 85
#define DEFAULT_MAX_HEAT_TIME 7200

// Prompts for LCD
#define PROMPT_STATUS_1 "Cur.:   "
#define PROMPT_STATUS_2 "Req.:   "
#define PROMPT_STATUS_3 "Status: "
#define PROMPT_MENU_1   "Hyst.:     "
#define PROMPT_MENU_2   "Min. tmp.: "
#define PROMPT_MENU_3   "Max. tmp.: "
#define PROMPT_MENU_4   "Max. run.: "

#endif
