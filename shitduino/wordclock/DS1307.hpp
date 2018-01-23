#include "utils.h"
#include "RTClib.h"

#ifndef DS1307_HPP
#define DS1307_HPP

#define DS1307_ADDRESS       0x68

void init_DS1307();
timestamp_t get_time(void);
void set_time(DateTime timestamp);


#endif /* DS1307_HPP */
