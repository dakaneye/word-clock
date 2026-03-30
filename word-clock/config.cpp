// word-clock/config.cpp
#include "config.h"

// Arduino Mega pin for each word — index by Word enum
// Pins match spec: grouped by physical MOSFET module on back of board.
// Enum order != module order. That's fine — this array maps enum -> pin.
const int WORD_PINS[NUM_WORDS] = {
  2,   // W_IT_IS      — Module 1, CH1
  3,   // W_TEN_MIN    — Module 1, CH2
  4,   // W_HALF       — Module 1, CH3
  5,   // W_QUARTER    — Module 1, CH4
  6,   // W_TWENTY     — Module 2, CH1
  7,   // W_FIVE_MIN   — Module 2, CH2
  8,   // W_MINUTES    — Module 2, CH3
  9,   // W_PAST       — Module 2, CH4
  10,  // W_TO         — Module 3, CH1
  22,  // W_ONE        — Module 4, CH1
  26,  // W_TWO        — Module 5, CH1
  13,  // W_THREE      — Module 3, CH4 (pin 13 = onboard LED, safe for output)
  24,  // W_FOUR       — Module 4, CH3
  33,  // W_FIVE_HR    — Module 6, CH4
  30,  // W_SIX        — Module 6, CH1
  28,  // W_SEVEN      — Module 5, CH3
  27,  // W_EIGHT      — Module 5, CH2
  29,  // W_NINE       — Module 5, CH4
  34,  // W_TEN_HR     — Module 7, CH1
  23,  // W_ELEVEN     — Module 4, CH2
  31,  // W_TWELVE     — Module 6, CH2
  35,  // W_OCLOCK     — Module 7, CH2
  36,  // W_AM         — Module 7, CH3
  37,  // W_PM         — Module 7, CH4
  11,  // W_HAPPY      — Module 3, CH2
  12,  // W_BIRTH      — Module 3, CH3
  32,  // W_CHELSEA    — Module 6, CH3
  25,  // W_DAY        — Module 4, CH4
};
