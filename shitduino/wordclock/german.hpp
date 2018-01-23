#ifndef GERMAN_H
#define GERMAN_H

#include "wordclock.hpp"
#include "RTClib.h"

/*
    ESKISTAFÜNF-\
  /-ZEHNZWANZIG-/
  \-DREIVIERTEL-\
  /-VORFUNKNACH-/
  \-HALBAELFÜNF-\
  /-EINSXAMZWEI-/
  \-DREIAUJVIER-\
  /-SECHSNLACHT-/
  \-SIEBENZWÖLF-\
  ->ZEHNEUNKUHR-/
*/

// array of color pixel that make up the character matrix
typedef union {
  matrix_t matrix;
  struct {
    // in order
    rgb_color_t zeh_h[3];
    rgb_color_t n_h[1];
    rgb_color_t eun_h[3];
    rgb_color_t _6[1];
    rgb_color_t uhr[3];
    // reversed
    rgb_color_t zwolf_h[5];
    rgb_color_t sieben_h[6];
    // in order
    rgb_color_t sechs_h[5];
    rgb_color_t _5[2];
    rgb_color_t acht_h[4];
    // reversed
    rgb_color_t vier_h[4];
    rgb_color_t _4[3];
    rgb_color_t drei_h[4];
    // in order
    rgb_color_t eins_h[4];
    rgb_color_t _3[3];
    rgb_color_t zwei_h[4];
    // reversed
    rgb_color_t unf_h[3];
    rgb_color_t f_h[1];
    rgb_color_t el_h[2];
    rgb_color_t _2[1];
    rgb_color_t halb[4];
    // in order
    rgb_color_t vor[3];
    rgb_color_t funk[4];
    rgb_color_t nach[4];
    // reversed
    rgb_color_t viertel[7];
    rgb_color_t drei[4];
    // in order
    rgb_color_t zehn_m[4];
    rgb_color_t zwanzig_m[7];
    // reversed
    rgb_color_t funf_m[4];
    rgb_color_t _1[1];
    rgb_color_t ist[3];
    rgb_color_t _0[1];
    rgb_color_t es[2];
  } words;
} german_matrix_t;

matrix_t timestamp2matrix_german(DateTime timestamp, colorscheme_t colorscheme);


#endif /* GERMAN_H */
