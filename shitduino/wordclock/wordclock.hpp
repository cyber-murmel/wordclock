#ifndef WORDCLOCK_HPP
#define WORDCLOCK_HPP

#include <stdint.h>
#include "utils.h"
#include "RTClib.h"


/* ---- DEFINES ---- */

#define NUM_PIXEL_COLS    (11)
#define NUM_PIXEL_LINES   (10)
#define NUM_PIXELS        (NUM_PIXEL_COLS * NUM_PIXEL_LINES)
#define EEPROM_COLOR_ADDRESS 0x00


/* ---- TYPEDEFS ---- */

typedef union {
  uint32_t u32;
  struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
  } bgr;
} rgb_color_t;

typedef struct {
  rgb_color_t it_is_color;
  rgb_color_t minute_color;
  rgb_color_t to_past_color;
  rgb_color_t half_color;
  rgb_color_t hour_color;
  rgb_color_t oclock_color;
} colorscheme_t;

typedef struct {
  rgb_color_t pixels[NUM_PIXELS];
} matrix_t;


/* ---- PUBLIC FUNCTIONS ---- */

/** Function to initialize the word clock
 *  
 *  Sets up the Neo Pixels, load the color scheme from last time
 * 
 * @param timestamp         time to display
 * @param colorsheme        colorscheme to use
 * @param timestamp2matrix  function to convert timestamp into displayable matrix
 */
void init_wordclock(uint8_t pixel_pin);

/** Changes to the next color scheme
 *
 */
void incremen_color_scheme(void);

/**
 * 
 * @param timestamp         time to display
 * @param colorsheme        colorscheme to use
 * @param timestamp2matrix  function to convert timestamp into displayable matrix
 */
void show_time(DateTime timestamp, matrix_t(timestamp2matrix(DateTime, colorscheme_t)));

#endif /* WORDCLOCK_HPP */

