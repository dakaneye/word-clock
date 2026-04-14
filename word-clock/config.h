// word-clock/config.h
#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// CUSTOMIZE THIS SECTION for your build.
// See docs/customization.md for a walkthrough.
// ============================================================

// Birthday: month and day to activate the birthday message
const int BIRTHDAY_MONTH = 5;  // May
const int BIRTHDAY_DAY = 4;

// How long each phase of the birthday alternation lasts (ms)
const unsigned long BIRTHDAY_CYCLE_MS = 5000;

// ============================================================
// WORD GRID — one enum value per word group on the clock face.
// Each word maps to one Arduino digital pin (see config.cpp).
// The Arduino drives LEDs directly — no MOSFET modules needed.
// Board is wired common cathode: bus wire = GND, word wires = anodes.
// ============================================================

enum Word {
  // Time words
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

  // Birthday words (HAPPY BIRTH DAY CHELSEA) — physically wired
  // as one daisy-chained group sharing a single anode wire
  W_BIRTHDAY,

  NUM_WORDS
};

// ============================================================
// PIN ASSIGNMENTS — maps each word to an Arduino Mega digital pin.
// Each pin drives the word's anode wire directly (HIGH = word on).
// The common cathode bus wire connects to Arduino GND.
// Defined in config.cpp. Change there if you wire differently.
// ============================================================

extern const int WORD_PINS[NUM_WORDS];

// Button pins (wire each between the pin and GND)
const int PIN_BTN_HOUR = 38;
const int PIN_BTN_MIN = 39;

// DS3231 RTC uses I2C: SDA=20, SCL=21 (hardwired on Mega)

#endif
