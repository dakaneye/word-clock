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
// Grouped by physical MOSFET module position on back of board
const int WORD_PINS[NUM_WORDS] = {
  // Pins match spec: grouped by physical MOSFET module on back of board.
  // Enum order != module order. That's fine — this array maps enum -> pin.

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
  13,  // W_THREE      — Module 3, CH4
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
