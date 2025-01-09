#ifndef DATALOG_H
#define DATALOG_H
#include <SPI.h>
#include <SD.h>

constexpr int SAMPLE_LIMIT = 6000;          // Number of samples before blinking LED
constexpr int SD_OK = 0;
constexpr int SD_ERR_INIT = 1;
constexpr int SD_ERR_NO_SPACE = 2;

constexpr int SUCCESS = 0;
constexpr int WRITE_ERR =1;
constexpr int MAX_SAMPLE_COUNT_REACHED = 2;
extern int sample_counter;                // Counter to track written samples
extern File output_file;

int SD_init(const int chipSelect);
String format_data(const float* piezoValues, const int len);
int save_data(const float* piezoValues, const int num_values, const int led_pin);

#endif