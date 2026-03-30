// word-clock/config.h
#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// CUSTOMIZE THIS SECTION for your build.
// See docs/customization.md for a walkthrough.
// ============================================================

// The name word on your clock face. Change the enum value and
// update birthday.cpp to use your new enum value.
// Example: rename W_CHELSEA to W_ALEX, update birthdayWords().
#define W_NAME W_CHELSEA

// Birthday: month and day to activate the birthday message
const int BIRTHDAY_MONTH = 5;  // May
const int BIRTHDAY_DAY = 4;

// How long each phase of the birthday alternation lasts (ms)
const unsigned long BIRTHDAY_CYCLE_MS = 5000;

// ============================================================
// WORD GRID — one enum value per word group on the clock face.
// If you design a different front panel, adjust this enum and
// update time_to_words.cpp + config.cpp to match.
// ============================================================

enum Word {
  // Time words — shared across all word clock builds
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

  // Birthday/name words — change these for your recipient
  W_HAPPY,
  W_BIRTH,
  W_CHELSEA,  // Replace with your recipient's name
  W_DAY,

  NUM_WORDS
};

// ============================================================
// PIN ASSIGNMENTS — maps each word to an Arduino Mega pin.
// Defined in config.cpp. Change there if you wire differently.
// ============================================================

extern const int WORD_PINS[NUM_WORDS];

// Button pins (wire each between the pin and GND)
const int PIN_BTN_HOUR = 38;
const int PIN_BTN_MIN = 39;

// DS3231 RTC uses I2C: SDA=20, SCL=21 (hardwired on Mega)

#endif
