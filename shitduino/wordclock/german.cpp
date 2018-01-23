#include "german.hpp"
#include "utils.h"
#include "RTClib.h"
#include <Arduino.h>

matrix_t timestamp2matrix_german(DateTime timestamp, colorscheme_t colorscheme) {
  german_matrix_t german_matrix;
  uint8_t hour_value;
  
  // clear matrix
  FILL32(german_matrix.matrix.pixels, 0);

  
  // ES IST is always on
  FILL32(german_matrix.words.es, colorscheme.it_is_color.u32);
  FILL32(german_matrix.words.ist, colorscheme.it_is_color.u32);
  
  switch (timestamp.minute() / 5) {
    case 0:
      break;
    case 1: case 2: case 3: case 4: case 7: case 8:
      FILL32(german_matrix.words.nach, colorscheme.to_past_color.u32);
      break;
    case 5: case 10:  case 11:
      FILL32(german_matrix.words.vor, colorscheme.to_past_color.u32);
      break;
    case 6: case 9:
      break;
  }
  
  hour_value = timestamp.hour() + (((timestamp.minute() / 5)<5) ? 0 : 1);

  // minute maping
  switch (timestamp.minute() / 5) {
    case 0: // Uhr
    default:
      FILL32(german_matrix.words.uhr, colorscheme.oclock_color.u32);
      break;
    case 1: // Fuenf nach
      FILL32(german_matrix.words.funf_m, colorscheme.minute_color.u32);
      break;
    case 2: // Zehn nach
      FILL32(german_matrix.words.zehn_m, colorscheme.minute_color.u32);
      break;
    case 3: // Viertel nach
      FILL32(german_matrix.words.viertel, colorscheme.minute_color.u32);
      break;
    case 4: // Zwangzig nach
      FILL32(german_matrix.words.zwanzig_m, colorscheme.minute_color.u32);
      break;
    case 5: // Fuenf vor Halb +1
      FILL32(german_matrix.words.funf_m, colorscheme.minute_color.u32);
      FILL32(german_matrix.words.halb, colorscheme.half_color.u32);
      break;
    case 6: // Halb +1
      FILL32(german_matrix.words.halb, colorscheme.half_color.u32);
      break;
    case 7: // Fuenf nach Halb +1
      FILL32(german_matrix.words.funf_m, colorscheme.minute_color.u32);
      FILL32(german_matrix.words.halb, colorscheme.half_color.u32);
      break;
    case 8: // Zehn nach Halb +1
      FILL32(german_matrix.words.zehn_m, colorscheme.minute_color.u32);
      FILL32(german_matrix.words.halb, colorscheme.half_color.u32);
      break;
    case 9: // Dreiviertel +1
      FILL32(german_matrix.words.drei, colorscheme.minute_color.u32);
      FILL32(german_matrix.words.viertel, colorscheme.minute_color.u32);
      break;
    case 10: // Zehn vor +1
      FILL32(german_matrix.words.zehn_m, colorscheme.minute_color.u32);
      break;
    case 11: // Fuenf vor +1
      FILL32(german_matrix.words.funf_m, colorscheme.minute_color.u32);
      break;
  }

  // hour mapping
  hour_value %= 12;
  switch (hour_value) {
    case 0:
      FILL32(german_matrix.words.zwolf_h, colorscheme.hour_color.u32);
      break;
    case 1:
      FILL32(german_matrix.words.eins_h, colorscheme.hour_color.u32);
      break;
    case 2:
      FILL32(german_matrix.words.zwei_h, colorscheme.hour_color.u32);
      break;
    case 3:
      FILL32(german_matrix.words.drei_h, colorscheme.hour_color.u32);
      break;
    case 4:
      FILL32(german_matrix.words.vier_h, colorscheme.hour_color.u32);
      break;
    case 5:
      FILL32(german_matrix.words.f_h, colorscheme.hour_color.u32);
      FILL32(german_matrix.words.unf_h, colorscheme.hour_color.u32);
      break;
    case 6:
      FILL32(german_matrix.words.sechs_h, colorscheme.hour_color.u32);
      break;
    case 7:
      FILL32(german_matrix.words.sieben_h, colorscheme.hour_color.u32);
      break;
    case 8:
      FILL32(german_matrix.words.acht_h, colorscheme.hour_color.u32);
      break;
    case 9:
      FILL32(german_matrix.words.n_h, colorscheme.hour_color.u32);
      FILL32(german_matrix.words.eun_h, colorscheme.hour_color.u32);
      break;
    case 10:
      FILL32(german_matrix.words.zeh_h, colorscheme.hour_color.u32);
      FILL32(german_matrix.words.n_h, colorscheme.hour_color.u32);
      break;
    case 11:
      FILL32(german_matrix.words.el_h, colorscheme.hour_color.u32);
      FILL32(german_matrix.words.f_h, colorscheme.hour_color.u32);
      break;
  }
  
  return german_matrix.matrix;
}

