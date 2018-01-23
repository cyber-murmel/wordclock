#include <Wire.h>
#include <string.h>
#include "RTClib.h"
#include "utils.h"

// addresses

// pin configuration
#define TIME_BUTTON_PIN   (2)
#define COLOR_BUTTON_PIN  (3)
#define NEO_PIXEL_PIN     (4)

// LED matrix configuration
#include "wordclock.hpp"
#include "german.hpp"

/* -------- global varibales -------- */

RTC_DS1307 rtc;

/* -------- helper functions -------- */

void print_DateTime_hour_minute(DateTime timestamp) {
  char txt[16];
  sprintf(txt, "Time is %02d:%02d\n", timestamp.hour(), timestamp.minute());
  Serial.print(txt);

}

/* -------- mandatory functions -------- */
void setup() {
  static char debug[64];
  
  init_wordclock(NEO_PIXEL_PIN);
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC\n");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!\n");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  // initialize pins
  pinMode(TIME_BUTTON_PIN, INPUT);
  pinMode(COLOR_BUTTON_PIN, INPUT);
  
}

void loop() {
  static DateTime timestamp, timestamp_new;
  
  timestamp_new = rtc.now();
  
  if(!DateTime_intact(timestamp_new)) {
    Serial.print("timestamp not intact\n");
    timestamp = DateTime(0);
  }
  else {
    if((timestamp_new.minute() != timestamp.minute()) ||
       (timestamp_new.hour()   != timestamp.hour())) {
      timestamp = timestamp_new;
      print_DateTime_hour_minute(timestamp);
      show_time(timestamp, timestamp2matrix_german);
    }
  }

  if (digitalRead(COLOR_BUTTON_PIN) == HIGH) {
    Serial.println("+++ Colour Button Pressed +++");
    incremen_color_scheme();
    show_time(timestamp, timestamp2matrix_german);
    while(digitalRead(COLOR_BUTTON_PIN) == HIGH);
  }
  
  if (digitalRead(TIME_BUTTON_PIN) == HIGH) {
    Serial.println("+++ Time Button Pressed +++");
    
    timestamp = timestamp + TimeSpan(0, 0, 1, 0);
    print_DateTime_hour_minute(timestamp);
    show_time(timestamp, timestamp2matrix_german);
    
    // sense for long press
    for(int i = 0; i < 1000 && digitalRead(TIME_BUTTON_PIN); i++) {
      delay(1);
    }
    if(digitalRead(TIME_BUTTON_PIN)) {
      Serial.println("+++ Time Button Longpressed +++");
    }
    while(digitalRead(TIME_BUTTON_PIN)) {
      timestamp = timestamp + TimeSpan(0, 0, 1, 0);// increment by 1 minute
      print_DateTime_hour_minute(timestamp);
      show_time(timestamp, timestamp2matrix_german);
      delay(50);
    }
    rtc.adjust(timestamp);
  }
  delay(100);
}


