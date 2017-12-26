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
  rgb_color_t hour_color;
  rgb_color_t oclock_color;
} colorscheme_t;


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
  rgb_color_t pixels[NUM_PIXELS];
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
    rgb_color_t acht_h[5];
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
    rgb_color_t ist[6];
    rgb_color_t _0[1];
    rgb_color_t es[2];
  } words;
} matrix_t;

const colorscheme_t colorschemes[] = {
  {
    .it_is_color   = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0x00, .g = 0x7f, .r = 0x7f}},
    .to_past_color = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .hour_color    = {.bgr = {.b = 0x7f, .g = 0x7f, .r = 0x00}},
    .oclock_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}}
  },
  {
    .it_is_color   = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .to_past_color = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .hour_color    = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}},
    .oclock_color  = {.bgr = {.b = 0xff, .g = 0xff, .r = 0xff}}
  },
  {
    .it_is_color   = {.bgr = {.b = 0x00, .g = 0x00, .r = 0xff}},
    .minute_color  = {.bgr = {.b = 0x00, .g = 0x7f, .r = 0x7f}},
    .to_past_color = {.bgr = {.b = 0x00, .g = 0xff, .r = 0x00}},
    .hour_color    = {.bgr = {.b = 0x7f, .g = 0x7f, .r = 0x00}},
    .oclock_color  = {.bgr = {.b = 0xff, .g = 0x00, .r = 0x00}}
  }
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIXEL_PIN, NEO_GRB | NEO_KHZ800);

volatile unsigned int colorscheme_number = 0;


/* -------- fucntion declaration -------- */

timestamp_t get_time(void);
void increment_time();
void set_time(timestamp_t timestamp);

inline unsigned int load_colorscheme_number();
void increment_color(void);
inline void store_colorscheme_number(unsigned int number);

matrix_t timestamp2matrix(timestamp_t timestamp, colorscheme_t colorscheme);
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

inline unsigned int load_colorscheme_number() {
  return EEPROM.read(EEPROM_COLOR_ADDRESS) % LENGTH(colorschemes);
}

inline void store_colorscheme_number(unsigned int number) {
  if(load_colorscheme_number() != number) {
    EEPROM.write(EEPROM_COLOR_ADDRESS, number);
  }
}

void increment_color(void) {
  Serial.println("+++ Colour Button Pressed +++");
  colorscheme_number = (colorscheme_number + 1) % LENGTH(colorschemes);
  store_colorscheme_number(colorscheme_number);
}


/* -------- DISPLAY -------- */

matrix_t timestamp2matrix(timestamp_t timestamp, colorscheme_t colorscheme) {
  matrix_t matrix;
  uint8_t hour_value;
  
  // clear matrix
  FILL32(matrix.pixels, 0);

  // IT IS is always on
  FILL32(matrix.words.es, colorscheme.it_is_color.u32);
  FILL32(matrix.words.ist, colorscheme.it_is_color.u32);

  switch (timestamp.minute / 5) {
    case 0:
      break;
    case 1: case 2: case 3: case 4: case 7: case 8:
      FILL32(matrix.words.nach, colorscheme.to_past_color.u32);
      break;
    case 5: case 10:  case 11:
      FILL32(matrix.words.vor, colorscheme.to_past_color.u32);
      break;
    case 6: case 9:
      break;
  }
  
  hour_value = timestamp.hour + (((timestamp.minute / 5)<5) ? 0 : 1);

  // minute maping
  switch (timestamp.minute / 5) {
    case 0: // Uhr
    default:
      FILL32(matrix.words.uhr, colorscheme.oclock_color.u32);
      break;
    case 1: // Fuenf nach
      FILL32(matrix.words.funf_m, colorscheme.minute_color.u32);
      break;
    case 2: // Zehn nach
      FILL32(matrix.words.zehn_m, colorscheme.minute_color.u32);
      break;
    case 3: // Viertel nach
      FILL32(matrix.words.viertel, colorscheme.minute_color.u32);
      break;
    case 4: // Zwangzig nach
      FILL32(matrix.words.zwanzig_m, colorscheme.minute_color.u32);
      break;
    case 5: // Fuenf vor Halb +1
      FILL32(matrix.words.funf_m, colorscheme.minute_color.u32);
      break;
    case 6: // Halb +1
      FILL32(matrix.words.halb, colorscheme.minute_color.u32);
      break;
    case 7: // Fuenf nach Halb +1
      FILL32(matrix.words.funf_m, colorscheme.minute_color.u32);
      break;
    case 8: // Zehn nach Halb +1
      FILL32(matrix.words.zehn_m, colorscheme.minute_color.u32);
      break;
    case 9: // Dreiviertel +1
      FILL32(matrix.words.drei, colorscheme.minute_color.u32);
      FILL32(matrix.words.viertel, colorscheme.minute_color.u32);
      break;
    case 10: // Zehn vor +1
      FILL32(matrix.words.zehn_m, colorscheme.minute_color.u32);
      break;
    case 11: // Fuenf vor +1
      FILL32(matrix.words.funf_m, colorscheme.minute_color.u32);
      break;
  }

  // hour mapping
  hour_value %= 12;
  switch (hour_value) {
    case 0:
      FILL32(matrix.words.zwolf_h, colorscheme.hour_color.u32);
      break;
    case 1:
      FILL32(matrix.words.eins_h, colorscheme.hour_color.u32);
      break;
    case 2:
      FILL32(matrix.words.zwei_h, colorscheme.hour_color.u32);
      break;
    case 3:
      FILL32(matrix.words.drei_h, colorscheme.hour_color.u32);
      break;
    case 4:
      FILL32(matrix.words.vier_h, colorscheme.hour_color.u32);
      break;
    case 5:
      FILL32(matrix.words.f_h, colorscheme.hour_color.u32);
      FILL32(matrix.words.unf_h, colorscheme.hour_color.u32);
      break;
    case 6:
      FILL32(matrix.words.sechs_h, colorscheme.hour_color.u32);
      break;
    case 7:
      FILL32(matrix.words.sieben_h, colorscheme.hour_color.u32);
      break;
    case 8:
      FILL32(matrix.words.acht_h, colorscheme.hour_color.u32);
      break;
    case 9:
      FILL32(matrix.words.n_h, colorscheme.hour_color.u32);
      FILL32(matrix.words.eun_h, colorscheme.hour_color.u32);
      break;
    case 10:
      FILL32(matrix.words.zeh_h, colorscheme.hour_color.u32);
      FILL32(matrix.words.n_h, colorscheme.hour_color.u32);
      break;
    case 11:
      FILL32(matrix.words.el_h, colorscheme.hour_color.u32);
      FILL32(matrix.words.f_h, colorscheme.hour_color.u32);
      break;
  }
  return matrix;
}

void show_time(timestamp_t timestamp) {
  static char debug_msg[128];
  matrix_t matrix;
  colorscheme_t colorscheme = colorschemes[colorscheme_number];

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
  
  matrix = timestamp2matrix(timestamp, colorscheme);
  
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, matrix.pixels[i].u32);
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
  colorscheme_number = load_colorscheme_number();
  
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

