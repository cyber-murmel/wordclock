#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#include "wordclock.hpp"
#include "utils.h"
#include "RTClib.h"

#define PIXEL_FLAGS (NEO_GRB | NEO_KHZ800)

static Adafruit_NeoPixel pixels;

/* ---- CONSTANTS ---- */

static const colorscheme_t colorschemes[] = {
  {
    .it_is_color   = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0x00, .g = 0x7f, .r = 0x7f}},
    .to_past_color = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .half_color    = {.bgr = {.b = 0x7f, .g = 0x7f, .r = 0x00}},
    .hour_color    = {.bgr = {.b = 0x7f, .g = 0x00, .r = 0x7f}},
    .oclock_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}}
  },
  {
    .it_is_color   = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0x00, .g = 0x00, .r = 0xff}},
    .to_past_color = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .half_color    = {.bgr = {.b = 0x00, .g = 0xff, .r = 0x00}},
    .hour_color    = {.bgr = {.b = 0xff, .g = 0x00, .r = 0x00}},
    .oclock_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}}
  },
  {
    .it_is_color   = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .to_past_color = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .half_color    = {.bgr = {.b = 0xFf, .g = 0xff, .r = 0xff}},
    .hour_color    = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .oclock_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}}
  },
  {
    .it_is_color   = {.bgr = {.b = 0x00, .g = 0x00, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0x00, .g = 0x7f, .r = 0x7f}},
    .to_past_color = {.bgr = {.b = 0x00, .g = 0xff, .r = 0x00}},
    .half_color    = {.bgr = {.b = 0x7f, .g = 0x7f, .r = 0x00}},
    .hour_color    = {.bgr = {.b = 0xff, .g = 0x00, .r = 0x00}},
    .oclock_color  = {.bgr = {.b = 0x7f, .g = 0x00, .r = 0x7f}}
  }
};


/* ---- PRIVATE VARIABLES ---- */
static unsigned int colorscheme_number;
static colorscheme_t colorscheme;


/* ---- PRIVATE FUNCTIONS ---- */

static inline unsigned int load_colorscheme_number() {
  return EEPROM.read(EEPROM_COLOR_ADDRESS) % LENGTH(colorschemes);
}

static inline void store_colorscheme_number(unsigned int number) {
  if(load_colorscheme_number() != number) {
    EEPROM.write(EEPROM_COLOR_ADDRESS, number);
  }
}


/* ---- PUBLIC FUNCTIONS ---- */

void init_wordclock(uint8_t pixel_pin) {
  pixels = Adafruit_NeoPixel(NUM_PIXELS, pixel_pin, PIXEL_FLAGS);
  pixels.begin();
  colorscheme_number = load_colorscheme_number();
  colorscheme = colorschemes[colorscheme_number];
}


void incremen_color_scheme(void) {
  colorscheme_number = (colorscheme_number + 1) % LENGTH(colorschemes);
  store_colorscheme_number(colorscheme_number);
  colorscheme = colorschemes[colorscheme_number];
}


void show_time(DateTime timestamp, matrix_t (* timestamp2matrix)(DateTime, colorscheme_t)) {
  static char debug_msg[128];
  matrix_t matrix = timestamp2matrix(timestamp, colorscheme);
  
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, matrix.pixels[i].u32);
  }
  pixels.show();
}
