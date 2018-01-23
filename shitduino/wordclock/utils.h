#ifndef UTILS_H
#define UTILS_H

#include "RTClib.h"

#define TO_BCD(X)         (((X/10)<<4) | (X%10))
#define FROM_BCD(X)       (((X>>4)*10) + (X % 16))
#define LENGTH(X)         (sizeof(X)/sizeof(X[0]))
#define FILL32(P, V)      fill_32(P, V, LENGTH(P))

typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t week_day;
  uint8_t month_day;
  uint8_t month;
  uint8_t year;
} timestamp_t;

static inline void fill_32(void * dst_p, uint32_t val, unsigned int len) {
  uint32_t *arr = dst_p;
  for (unsigned int i = 0; i < len; i++) {
    arr[i] = val;
  }
}

static inline bool DateTime_intact(DateTime timestamp) {
  bool result = 1;
  if(timestamp.second()        > 60) result = 0;
  if(timestamp.minute()        > 60) result = 0;
  if(timestamp.hour()          > 24) result = 0;
  if(timestamp.dayOfTheWeek()  >  7) result = 0;
  if(timestamp.day()           > 31) result = 0;
  if(timestamp.month()         > 12) result = 0;
  return result;
}

#endif /* UTILS_H */
