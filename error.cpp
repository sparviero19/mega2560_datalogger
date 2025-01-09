#include <Arduino.h>
#include "error.h"


void error(int code, int led_pin, const bool& control_var){
  /*
    Error codes to be translated to led blinking.
  */
  String s_err; 
  switch(code) {
    case 0:
      s_err = "sss";
      break;
    case 1:
      s_err = "sst";
      break;
    case 2:
      s_err = "sts";
      break;
    case 3: 
      s_err = "stt";
      break;
    case 4:
      s_err = "tss";
      break;
    case 5:
      s_err = "tst";
      break;
    case 9:
    default:
      s_err = "f";
  }
  //Serial.println(control_var);
  while(control_var==false){
    blink_led_with_pattern(s_err, led_pin);
    delay(1000);
  }
}


void three_blinks(int led_pin) {
  for (int i = 0; i < 3; i++) { // Blink LED 3 times
    digitalWrite(led_pin, HIGH);
    delay(200);
    digitalWrite(led_pin, LOW);
    delay(200);
  }
}

void blink_led_with_pattern(String& pattern, int led){
  /*
   Codes are computed as following:
    f -> fixed lignt
    s -> (dash) a long blink
    t -> (dot) a short blink

    Combination fo 's' and 't' are followed by a pause of 1000ms. 
    Dashes are 250ms HIGH, while dots are 125ms HIGH, both separated by 100ms LOW led values.
    */
  int len = pattern.length();
  // Serial.print("Len = ");
  // Serial.println(len);
  char c;
  for(int i=0; i<len; ++i){
    c = pattern[i];
    // Serial.println(c);
    switch(c){
      case 'f':
        digitalWrite(led, HIGH);
        return;
      case 's':
        digitalWrite(led, HIGH);
        delay(500);
        digitalWrite(led,LOW);
        delay(100);
        break;
      case 't':
        digitalWrite(led, HIGH);
        delay(250);
        digitalWrite(led,LOW);
        delay(100);
        break;
    }
      
  }
}