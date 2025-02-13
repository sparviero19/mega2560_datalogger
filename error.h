#ifndef ERROR_H
#define ERROR_H

// Error codes
const int GENERIC_ERROR = 9;
const int SD_ERROR = 0;
const int SERIAL_ERROR = 1;
const int DATA_WRITE_ERR = 2;
const int MAX_SAMPLE_ERR = 3;

void error(int code, int led_pin, const bool& control_var);
void three_blinks(int led_pin);
void blink_led_with_pattern(String& pattern, int led);

#endif