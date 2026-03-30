// word-clock/config.h
#ifndef CONFIG_H
#define CONFIG_H

// Word indices — used as array index into WORD_PINS[]
enum Word {
  W_IT_IS = 0,
  W_TEN_MIN,
  W_HALF,
  W_QUARTER,
  W_TWENTY,
  W_FIVE_MIN,
  W_MINUTES,
  W_PAST,
  W_TO,
  W_ONE,
  W_TWO,
  W_THREE,
  W_FOUR,
  W_FIVE_HR,
  W_SIX,
  W_SEVEN,
  W_EIGHT,
  W_NINE,
  W_TEN_HR,
  W_ELEVEN,
  W_TWELVE,
  W_OCLOCK,
  W_AM,
  W_PM,
  W_HAPPY,
  W_BIRTH,
  W_CHELSEA,
  W_DAY,
  NUM_WORDS  // = 28
};

// Arduino Mega pin for each word — index by Word enum
// Defined in config.cpp. Grouped by physical MOSFET module position.
extern const int WORD_PINS[NUM_WORDS];

// Button pins
const int PIN_BTN_HOUR = 38;
const int PIN_BTN_MIN = 39;

// DS3231 RTC uses I2C: SDA=20, SCL=21 (hardwired on Mega)

// Birthday config
const int BIRTHDAY_MONTH = 5;  // May
const int BIRTHDAY_DAY = 4;

// Display timing
const unsigned long BIRTHDAY_CYCLE_MS = 5000;  // 5 seconds per phase

#endif
