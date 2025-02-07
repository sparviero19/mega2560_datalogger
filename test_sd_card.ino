/*
TODO: some descriptions of the work
*/
#include <Arduino.h>
#include "error.h"
#include "datalog.h"
#include "test.hpp"

#define TEST

const int LED_PIN1 = LED_BUILTIN;
const int LED_PIN2 = 9; // a red led should be connected to this pin to communicate error codes. 
const int ERROR_PIN = LED_PIN2;
const int BUTTON_PIN = 8;

// const int RUN_BUTTON = ; // button to start and stop recording operations
const int chipSelect = 53;             // SD card CS pin
const int PIEZO_PINS[] = {A0, A1, A2, A3, A4};
const int NUM_PIEZO = 5;
float piezo_values[NUM_PIEZO];

// State machine variables
bool reset = false;
bool record = false;
bool pause = false;
int button_pressed = 0;

void setup() {

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  digitalWrite(LED_PIN1,LOW);
  digitalWrite(LED_PIN2, LOW);
  
  Serial.begin(57600);
  float timeout = 5000;
  float time = 0;
  while (!Serial){
    delay(500);
    time+=500;
    if (time>=timeout) error(SERIAL_ERROR, ERROR_PIN, reset);
  }
  Serial.println("P1 P2 P3 P4 P5");

  // SD card initialization
  int code = SD_init(chipSelect);
  if (code != SD_OK) {
    Serial.println("SD card initialization failed!");
    error(SD_ERROR, ERROR_PIN, reset);
  }
  Serial.println("SD card initialized.");
  //SD.remove("data.csv");

  //SETUP Timer for reading sensors
  //TCCR2A|=(1<<WGM01); // Timer compare mode

}

ISR(TIMER0_COMPA_vect){    //This is the interrupt request 
}


inline void read_piezos(float* values, int rate) {
  /*
    Read and accumulate analog signals for a desired minimum rate in Hz. In reality, some margin is kept for 
    the other computations in the loop. This is done until an interrupt based solution for the reading is implemented.
  */
  // TODO: all this code will have to be handled trough timer interrupts
  const int overhead = 10; //millisecs
  int period = (1000-overhead)/rate;
  clear_piezos(values);
  for(int j=0; j<rate; ++j){
    for(int i=0; i<NUM_PIEZO; ++i) {
      values[i] += analogRead(PIEZO_PINS[i]);
    }
    delay(period);
  }
  for(int i=0; i<NUM_PIEZO; ++i)
    values[i] /= rate;
}

void clear_piezos(float* values) {
  /* 
    set to 0 the sampled values
  */
  memset(values, 0, sizeof(values));
}

void print_to_serial(float* piezoValues){

  for (int i = 0; i < NUM_PIEZO; i++) {
    Serial.print(piezoValues[i]);
    Serial.print('\t');
  }
  Serial.println();
}


#if defined(TEST)
bool done = false;
void loop() {
  if (! done) {
    TestDataFile test_df;
    test_df.test_construction();
    done = true;
  }
  else{
    Serial.println("program terminated");
  }
  while(true);
}

#else

void loop() {

  button_pressed = digitalRead(BUTTON_PIN);
  while(digitalRead(BUTTON_PIN)==HIGH);
  // Serial.print("button value: ");
  // Serial.println(button_pressed);
  if (button_pressed == HIGH) {
    Serial.println("Program started!");
    record = true;
    button_pressed = LOW;
  }

  while(record) {

    read_piezos(piezo_values, 10);
    print_to_serial(piezo_values);
    int out = save_data(piezo_values, NUM_PIEZO, LED_PIN1);
    switch(out){
      case SUCCESS:
        break;
      case MAX_SAMPLE_COUNT_REACHED:
        error(MAX_SAMPLE_ERR, LED_PIN1, reset);
        break;
      case WRITE_ERR:
        error(DATA_WRITE_ERR, LED_PIN1, reset);
        break;
      default:
        error(GENERIC_ERROR, LED_PIN1, reset);
    }
    //delay(500);
    button_pressed = digitalRead(BUTTON_PIN);
    while(digitalRead(BUTTON_PIN)==HIGH);

    if (button_pressed == HIGH) {
      Serial.println("Program stopped.");
      record = false;
      button_pressed = LOW;
    }
  }
  
}
#endif

