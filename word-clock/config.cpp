// word-clock/config.cpp
#include "config.h"

// Arduino Mega pin for each word — index by Word enum.
// Each pin drives the word's anode wire directly (HIGH = on).
// The common cathode bus wire connects to Arduino GND.
const int WORD_PINS[NUM_WORDS] = {
  2,   // W_IT_IS
  3,   // W_TEN_MIN
  4,   // W_HALF
  5,   // W_QUARTER
  6,   // W_TWENTY
  7,   // W_FIVE_MIN
  8,   // W_MINUTES
  9,   // W_PAST
  10,  // W_TO
  22,  // W_ONE
  26,  // W_TWO
  13,  // W_THREE (pin 13 = onboard LED, blinks on boot)
  24,  // W_FOUR
  33,  // W_FIVE_HR
  30,  // W_SIX
  28,  // W_SEVEN
  27,  // W_EIGHT
  29,  // W_NINE
  34,  // W_TEN_HR
  23,  // W_ELEVEN
  31,  // W_TWELVE
  35,  // W_OCLOCK
  36,  // W_AM
  37,  // W_PM
  11,  // W_BIRTHDAY (HAPPY BIRTH DAY CHELSEA — single daisy-chained group)
};
