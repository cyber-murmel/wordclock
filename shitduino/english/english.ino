#include <EEPROM.h>
#include <Wire.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>

// addresses
#define DS1307_ADDRESS       0x68
#define EEPROM_COLOR_ADDRESS 0x00

// pin configuration
#define TIME_BUTTON_PIN   (2)
#define COLOR_BUTTON_PIN  (3)
#define NEO_PIXEL_PIN     (4)

// LED matrix configuration
#define NUM_PIXEL_COLS    (11)
#define NUM_PIXEL_LINES   (11)
#define NUM_PIXELS        (NUM_PIXEL_COLS * NUM_PIXEL_LINES)

// helper functions
#define TO_BCD(X)         (((X/10)<<4) | (X%10))
#define FROM_BCD(X)       (((X>>4)*10) + (X % 16))
#define LENGTH(X)         (sizeof(X)/sizeof(X[0]))
#define FILL32(P, V)      fill_32(P, V, LENGTH(P))

static inline void fill_32(void * dst_p, uint32_t val, unsigned int len) {
  uint32_t *arr = dst_p;
  for (unsigned int i = 0; i < len; i++) {
    arr[i] = val;
  }
}


/* -------- type definitions -------- */

typedef union {
  uint32_t sum;
  #pragma pack()
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  } rgb;
} rgb_color_t;

typedef struct {
  rgb_color_t it_is_color;
  rgb_color_t minute_color;
  rgb_color_t to_past_color;
  rgb_color_t hour_color;
  rgb_color_t oclock_color;
} color_schemes_t;


typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t week_day;
  uint8_t month_day;
  uint8_t month;
  uint8_t year;
} timestamp_t;

/* -------- global varibales -------- */

/*
    ITEISFTLVNE-\
  /-JQUARTERCKO-/
  \-TWENTYXFIVE-\
  /-HALFCTENETO-/
  \-PASTBSEVENL-\
  /-ONETWOTHREE-/
  \-FOURFIVESIX-\
  /-NINEKTWELVE-/
  \-EIGHTELEVEN-\
  ->TENDBOCLOCK-/
*/

// array of color pixel that make up the character matrix
#pragma pack()
union {
  rgb_color_t pixels[NUM_PIXELS];
  struct {
    // in order
    rgb_color_t ten_h[3];
    rgb_color_t _11[2];
    rgb_color_t oclock[6];
    // reversed
    rgb_color_t eleven_h[6];
    rgb_color_t eight_h[5];
    // in order
    rgb_color_t nine_h[4];
    rgb_color_t _10[1];
    rgb_color_t twelve_h[6];
    // reversed
    rgb_color_t six_h[3];
    rgb_color_t five_h[4];
    rgb_color_t four_h[4];
    // in order
    rgb_color_t one_h[3];
    rgb_color_t two_h[3];
    rgb_color_t three_h[5];
    // reversed
    rgb_color_t _9[1];
    rgb_color_t seven_h[5];
    rgb_color_t _8[1];
    rgb_color_t past[4];
    // in order
    rgb_color_t half[4];
    rgb_color_t _6[1];
    rgb_color_t ten_m[3];
    rgb_color_t _7[1];
    rgb_color_t to[2];
    // reversed
    rgb_color_t five_m[4];
    rgb_color_t _5[1];
    rgb_color_t twenty_m[6];
    // in order
    rgb_color_t _4[1];
    rgb_color_t quarter[7];
    rgb_color_t _3[1];
    // reversed
    rgb_color_t _2[6];
    rgb_color_t is[2];
    rgb_color_t _1[1];
    rgb_color_t it[2];
  } words;
} matrix;

color_schemes_t color_schemes[] = {
  {
    .it_is_color   = {.rgb = {0xff, 0x00, 0x00}},
    .minute_color  = {.rgb = {0xff, 0x00, 0x00}},
    .to_past_color = {.rgb = {0xff, 0x00, 0x00}},
    .hour_color    = {.rgb = {0xff, 0x00, 0x00}},
    .oclock_color  = {.rgb = {0xff, 0x00, 0x00}}
  },
  {
    .it_is_color   = {.rgb = {0xff, 0x00, 0x00}},
    .minute_color  = {.rgb = {0xff, 0x00, 0x00}},
    .to_past_color = {.rgb = {0xff, 0x00, 0x00}},
    .hour_color    = {.rgb = {0xff, 0x00, 0x00}},
    .oclock_color  = {.rgb = {0xff, 0x00, 0x00}}
  }
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIXEL_PIN, NEO_GRB | NEO_KHZ800);

unsigned int color_scheme_number = 0;


/* -------- fucntion declaration -------- */

timestamp_t get_time(void);
void increment_time();
void set_time(timestamp_t timestamp);

inline unsigned int load_color_scheme_number();
void increment_color(void);
inline void store_color_scheme_number(unsigned int number);

void show_time(timestamp_t timestamp);


/* -------- TIME -------- */

timestamp_t get_time(void) {
  timestamp_t timestamp;
  // Reset the register pointer
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);
  //Need to read the sum packet of information even though Hour is only needed
  timestamp.second    = FROM_BCD(Wire.read()); // seconds
  timestamp.minute    = FROM_BCD(Wire.read()); // minute
  timestamp.hour      = FROM_BCD(Wire.read() & 0b111111); //24 hour time
  timestamp.week_day  = FROM_BCD(Wire.read()); //0-6 -> Sunday - Saturday
  timestamp.month_day = FROM_BCD(Wire.read());
  timestamp.month     = FROM_BCD(Wire.read());
  timestamp.year      = FROM_BCD(Wire.read());

  return timestamp;
}

void set_time(timestamp_t timestamp) {
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00); //stop Oscillator

  Wire.write(TO_BCD(timestamp.second));
  Wire.write(TO_BCD(timestamp.minute));
  Wire.write(TO_BCD(timestamp.hour));
  Wire.write(TO_BCD(timestamp.week_day));
  Wire.write(TO_BCD(timestamp.month_day));
  Wire.write(TO_BCD(timestamp.month));
  Wire.write(TO_BCD(timestamp.year));

  Wire.write(0x00); //start
  Wire.endTransmission();
}

void increment_time() {
  timestamp_t timestamp = get_time();

  // increment time
  timestamp.minute++;
  if (timestamp.minute >= 60) {
    timestamp.hour++;
    timestamp.hour %= 24;
    timestamp.minute %= 60;
  }

  set_time(timestamp);
}


/* -------- COLOR SCHEME -------- */

inline unsigned int load_color_scheme_number() {
  return EEPROM.read(EEPROM_COLOR_ADDRESS) % LENGTH(color_schemes);
}

inline void store_color_scheme_number(unsigned int number) {
  if(load_color_scheme_number() != number) {
    EEPROM.write(EEPROM_COLOR_ADDRESS, number);
  }
}

void increment_color(void) {
  Serial.println("+++ Colour Button Pressed +++");
  color_scheme_number = (color_scheme_number + 1) % LENGTH(color_schemes);
  store_color_scheme_number(color_scheme_number);
}


/* -------- DISPLAY -------- */

void show_time(timestamp_t timestamp) {
  static char debug_msg[128];
  color_schemes_t color_scheme = color_schemes[color_scheme_number];
  uint8_t hour_value;

  //Check to see if inputHour and inputMinute are within suitable range
  if (timestamp.hour < 0 || timestamp.hour > 23) {
    sprintf(debug_msg, "Error: Hour value is %04d.\n", timestamp.hour);
    Serial.print(debug_msg);
    return;
  }
  if (timestamp.minute < 0 || timestamp.minute > 59) {
    sprintf(debug_msg, "Error: Minute value is %04d.\n", timestamp.minute);
    Serial.print(debug_msg);
    return;
  }

  // clear matrix
  FILL32(matrix.pixels, 0);

  // IT IS is always on
  FILL32(matrix.words.it, color_scheme.it_is_color.sum);
  FILL32(matrix.words.is, color_scheme.it_is_color.sum);

  switch (timestamp.minute / 5) {
    case 0: case 1: case 2: case 3: case 4: case 5:
      FILL32(matrix.words.past, color_scheme.to_past_color.sum);
      hour_value = timestamp.hour;
      break;
    case 6: case 7: case 8: case 9: case 10:  case 11:
      FILL32(matrix.words.to, color_scheme.to_past_color.sum);
      hour_value = timestamp.hour + 1;
      break;
  }

  // minute maping
  switch (timestamp.minute / 5) {
    case 0:
    default:
      FILL32(matrix.words.oclock, color_scheme.oclock_color.sum);
      break;
    case 1:
    case 11:
      FILL32(matrix.words.five_m, color_scheme.minute_color.sum);
      break;
    case 2:
    case 10:
      FILL32(matrix.words.ten_m, color_scheme.minute_color.sum);
      break;
    case 3:
    case 9:
      FILL32(matrix.words.quarter, color_scheme.minute_color.sum);
      break;
    case 4:
    case 8:
      FILL32(matrix.words.twenty_m, color_scheme.minute_color.sum);
      break;
    case 5:
    case 7:
      FILL32(matrix.words.twenty_m, color_scheme.minute_color.sum);
      FILL32(matrix.words.five_m, color_scheme.minute_color.sum);
      break;
    case 6:
      FILL32(matrix.words.half, color_scheme.minute_color.sum);
  }

  // hour mapping
  hour_value %= 12;
  switch (hour_value) {
    case 0:
      FILL32(matrix.words.twelve_h, color_scheme.hour_color.sum);
      break;
    case 1:
      FILL32(matrix.words.one_h, color_scheme.hour_color.sum);
      break;
    case 2:
      FILL32(matrix.words.two_h, color_scheme.hour_color.sum);
      break;
    case 3:
      FILL32(matrix.words.three_h, color_scheme.hour_color.sum);
      break;
    case 4:
      FILL32(matrix.words.four_h, color_scheme.hour_color.sum);
      break;
    case 5:
      FILL32(matrix.words.five_h, color_scheme.hour_color.sum);
      break;
    case 6:
      FILL32(matrix.words.six_h, color_scheme.hour_color.sum);
      break;
    case 7:
      FILL32(matrix.words.seven_h, color_scheme.hour_color.sum);
      break;
    case 8:
      FILL32(matrix.words.eight_h, color_scheme.hour_color.sum);
      break;
    case 9:
      FILL32(matrix.words.nine_h, color_scheme.hour_color.sum);
      break;
    case 10:
      FILL32(matrix.words.ten_h, color_scheme.hour_color.sum);
      break;
    case 11:
      FILL32(matrix.words.eleven_h, color_scheme.hour_color.sum);
      break;
  }
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, matrix.pixels[i].sum);
  }
  pixels.show();
}

/* -------- mandatory functions -------- */
void setup() {
  // start I2C for RTC and UART
  Wire.begin();
  Serial.begin(9600);
  
  // initialize pins
  pinMode(TIME_BUTTON_PIN, INPUT);
  pinMode(COLOR_BUTTON_PIN, INPUT);
  
  // read color scheme number from non volatile memory
  color_scheme_number = load_color_scheme_number();
  
  // initialize NeoPixel library
  pixels.begin();
}

void loop() {
  static char debug[15];
  timestamp_t timestamp = get_time();

  sprintf(debug, "RTC: %02d:%02d\n", timestamp.hour, timestamp.minute);
  Serial.print(debug);

  show_time(timestamp);

  if (digitalRead(COLOR_BUTTON_PIN) == HIGH) {
    increment_color();
    while(digitalRead(COLOR_BUTTON_PIN) == HIGH);
  }
  if (digitalRead(TIME_BUTTON_PIN) == HIGH) {
    increment_time();
    delay(50);
  }
}

