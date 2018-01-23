#include <Wire.h>
#include <Arduino.h>
#include"DS1307.hpp"

static char debug[64];

void init_DS1307() {
  Wire.begin();
}

timestamp_t get_time(void) {
  timestamp_t timestamp;
  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write((uint8_t) 0);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);
  //Need to read the sum packet of information even though Hour is only needed
  timestamp.second    = FROM_BCD(Wire.read()); // seconds
  timestamp.minute    = FROM_BCD(Wire.read()); // minute
  timestamp.hour      = FROM_BCD(Wire.read() & 0b011111); //12 hour time
  timestamp.week_day  = FROM_BCD(Wire.read()); //0-6 -> Sunday - Saturday
  timestamp.month_day = FROM_BCD(Wire.read());
  timestamp.month     = FROM_BCD(Wire.read());
  timestamp.year      = FROM_BCD(Wire.read());

  sprintf(debug, "Read time as %04d-%02d-%02d %02d:%02d:%02d\n", timestamp.year, timestamp.month, timestamp.month_day, timestamp.hour, timestamp.minute, timestamp.second);
  Serial.print(debug);
  
  timestamp = {1, 1, 1, 1, 1, 1, 1};
  return timestamp;
}

void set_time(timestamp_t timestamp) {
  sprintf(debug, "Setting time to %04d-%02d-%02d %02d:%02d:%02d\n", timestamp.year, timestamp.month, timestamp.month_day, timestamp.hour, timestamp.minute, timestamp.second);
  Serial.print(debug);
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write((uint8_t) 0); //stop Oscillator

  Wire.write(TO_BCD(timestamp.second));// sec
  Wire.write(TO_BCD(timestamp.minute));// min
  Wire.write(TO_BCD(timestamp.hour));// hour
  Wire.write(TO_BCD(timestamp.week_day));// day
  Wire.write(TO_BCD(timestamp.month_day));// date
  Wire.write(TO_BCD(timestamp.month));// month
  Wire.write(TO_BCD(timestamp.year));// year

  Wire.write((uint8_t) 0); //start Oscillator
  Wire.endTransmission();
}

