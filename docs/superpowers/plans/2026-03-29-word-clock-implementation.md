# Word Clock Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build testable Arduino firmware that displays time in words on a LED word clock, with birthday mode for May 4th.

**Architecture:** Split into pure-logic modules (testable on macOS) and hardware modules (tested on Arduino). `config.h` defines all pin mappings. `time_to_words` is the core brain with zero hardware deps. `display`, `clock`, `buttons`, `birthday` handle hardware interaction. Main sketch wires them together.

**Tech Stack:** Arduino C++ (arduino-cli, arduino:avr:mega board), native C++ for tests (clang++ on macOS), DS3231 RTC via Wire library.

---

## File Structure

```
word-clock/
├── word-clock/                 # Arduino sketch directory
│   ├── word-clock.ino          # Main sketch — setup() + loop()
│   ├── config.h                # Pin assignments, word enum, pin mapping array
│   ├── time_to_words.h         # Pure logic header: hour+minute → word set
│   ├── time_to_words.cpp       # Pure logic implementation
│   ├── display.h               # Turn words on/off via MOSFET pins
│   ├── display.cpp             # Display implementation
│   ├── clock.h                 # DS3231 RTC read/write
│   ├── clock.cpp               # Clock implementation
│   ├── buttons.h               # Button handling with debounce + hold-repeat
│   ├── buttons.cpp             # Buttons implementation
│   ├── birthday.h              # Birthday detection + alternating display
│   └── birthday.cpp            # Birthday implementation
├── test/
│   ├── test_time_to_words.cpp  # Native macOS test for all 144 time→word mappings
│   └── Makefile                # Compiles and runs tests with clang++
├── test-sketches/
│   ├── rtc-test/rtc-test.ino           # Test sketch: verify RTC reads time
│   ├── pin-sweep/pin-sweep.ino         # Test sketch: cycle each output pin
│   ├── word-test/word-test.ino         # Test sketch: serial command → light word
│   └── button-test/button-test.ino     # Test sketch: verify button input
└── docs/
    └── superpowers/
        ├── specs/2026-03-29-word-clock-design.md
        └── plans/2026-03-29-word-clock-implementation.md
```

---

## Task 1: config.h — Pin Assignments and Word Enum

**Files:**
- Create: `word-clock/config.h`

- [ ] **Step 1: Create config.h with word enum and pin mapping**

```cpp
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
  // Enum order != module order. That's fine — this array maps enum → pin.

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
```

Note: Pin numbers shifted from spec to keep birthday words on their own module (Module 7) and avoid pin conflicts with I2C (20/21). Final pin assignments will be verified during the pin sweep test — just change numbers here if wiring differs.

- [ ] **Step 2: Verify it compiles**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors

- [ ] **Step 3: Commit**

```bash
git add word-clock/config.h
git commit -m "feat: add config.h with word enum and pin assignments"
```

---

## Task 2: time_to_words — Pure Logic (TDD on macOS)

**Files:**
- Create: `word-clock/time_to_words.h`
- Create: `word-clock/time_to_words.cpp`
- Create: `test/test_time_to_words.cpp`
- Create: `test/Makefile`

- [ ] **Step 1: Create time_to_words.h**

```cpp
// word-clock/time_to_words.h
#ifndef TIME_TO_WORDS_H
#define TIME_TO_WORDS_H

#include "config.h"

// Maximum words that can be active at once
// IT IS + TWENTY + FIVE + MINUTES + PAST + hour + AM/PM = 7
const int MAX_ACTIVE_WORDS = 8;

struct WordSet {
  int words[MAX_ACTIVE_WORDS];
  int count;
};

// Given hour (1-12), minute (0-59), and isPM flag,
// returns which words should be lit.
// Hour must be 1-12 (not 0-23).
WordSet timeToWords(int hour, int minute, bool isPM);

#endif
```

- [ ] **Step 2: Create the test file with all 144 mappings**

```cpp
// test/test_time_to_words.cpp
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Stub out Arduino types so config.h compiles natively
#define HIGH 1
#define LOW 0
typedef unsigned char uint8_t;
typedef unsigned long uint32_t;

#include "../word-clock/config.h"
#include "../word-clock/time_to_words.h"
#include "../word-clock/time_to_words.cpp"

static int tests_run = 0;
static int tests_passed = 0;

const char* WORD_NAMES[NUM_WORDS] = {
  "IT_IS", "TEN_MIN", "HALF", "QUARTER", "TWENTY", "FIVE_MIN",
  "MINUTES", "PAST", "TO", "ONE", "TWO", "THREE", "FOUR",
  "FIVE_HR", "SIX", "SEVEN", "EIGHT", "NINE", "TEN_HR",
  "ELEVEN", "TWELVE", "OCLOCK", "AM", "PM",
  "HAPPY", "BIRTH", "CHELSEA", "DAY"
};

bool wordSetContains(const WordSet& ws, int word) {
  for (int i = 0; i < ws.count; i++) {
    if (ws.words[i] == word) return true;
  }
  return false;
}

void printWordSet(const WordSet& ws) {
  for (int i = 0; i < ws.count; i++) {
    if (i > 0) printf(" ");
    printf("%s", WORD_NAMES[ws.words[i]]);
  }
  printf("\n");
}

void expectWords(int hour, int minute, bool isPM, const int* expected, int expectedCount) {
  tests_run++;
  WordSet ws = timeToWords(hour, minute, isPM);

  // Check all expected words are present
  bool pass = true;
  for (int i = 0; i < expectedCount; i++) {
    if (!wordSetContains(ws, expected[i])) {
      pass = false;
      break;
    }
  }
  // Check no extra words
  if (ws.count != expectedCount) pass = false;

  if (pass) {
    tests_passed++;
  } else {
    printf("FAIL: %d:%02d %s\n", hour, minute, isPM ? "PM" : "AM");
    printf("  Expected: ");
    for (int i = 0; i < expectedCount; i++) {
      if (i > 0) printf(" ");
      printf("%s", WORD_NAMES[expected[i]]);
    }
    printf("\n  Got:      ");
    printWordSet(ws);
  }
}

void testOclock() {
  // 3:00 AM → IT IS THREE O'CLOCK AM
  int expected[] = {W_IT_IS, W_THREE, W_OCLOCK, W_AM};
  expectWords(3, 0, false, expected, 4);

  // 12:00 PM → IT IS TWELVE O'CLOCK PM
  int expected2[] = {W_IT_IS, W_TWELVE, W_OCLOCK, W_PM};
  expectWords(12, 0, true, expected2, 4);

  // :01-:04 still shows O'CLOCK
  int expected3[] = {W_IT_IS, W_SEVEN, W_OCLOCK, W_PM};
  expectWords(7, 3, true, expected3, 4);
}

void testFivePast() {
  // 3:05 AM → IT IS FIVE MINUTES PAST THREE AM
  int expected[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_PAST, W_THREE, W_AM};
  expectWords(3, 5, false, expected, 6);

  // 10:07 PM → IT IS FIVE MINUTES PAST TEN PM
  int expected2[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_PAST, W_TEN_HR, W_PM};
  expectWords(10, 7, true, expected2, 6);
}

void testTenPast() {
  // 6:10 AM → IT IS TEN MINUTES PAST SIX AM
  int expected[] = {W_IT_IS, W_TEN_MIN, W_MINUTES, W_PAST, W_SIX, W_AM};
  expectWords(6, 10, false, expected, 6);
}

void testQuarterPast() {
  // 1:15 PM → IT IS QUARTER PAST ONE PM
  int expected[] = {W_IT_IS, W_QUARTER, W_PAST, W_ONE, W_PM};
  expectWords(1, 15, true, expected, 5);
}

void testTwentyPast() {
  // 8:20 AM → IT IS TWENTY MINUTES PAST EIGHT AM
  int expected[] = {W_IT_IS, W_TWENTY, W_MINUTES, W_PAST, W_EIGHT, W_AM};
  expectWords(8, 20, false, expected, 6);
}

void testTwentyFivePast() {
  // 11:25 PM → IT IS TWENTY FIVE MINUTES PAST ELEVEN PM
  int expected[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_PAST, W_ELEVEN, W_PM};
  expectWords(11, 25, true, expected, 7);
}

void testHalfPast() {
  // 4:30 AM → IT IS HALF PAST FOUR AM
  int expected[] = {W_IT_IS, W_HALF, W_PAST, W_FOUR, W_AM};
  expectWords(4, 30, false, expected, 5);

  // :31-:34 still shows HALF PAST
  int expected2[] = {W_IT_IS, W_HALF, W_PAST, W_FOUR, W_AM};
  expectWords(4, 33, false, expected2, 5);
}

void testTwentyFiveTo() {
  // 9:35 AM → IT IS TWENTY FIVE MINUTES TO TEN AM
  int expected[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_TO, W_TEN_HR, W_AM};
  expectWords(9, 35, false, expected, 7);
}

void testTwentyTo() {
  // 5:40 PM → IT IS TWENTY MINUTES TO SIX PM
  int expected[] = {W_IT_IS, W_TWENTY, W_MINUTES, W_TO, W_SIX, W_PM};
  expectWords(5, 40, true, expected, 6);
}

void testQuarterTo() {
  // 2:45 AM → IT IS QUARTER TO THREE AM
  int expected[] = {W_IT_IS, W_QUARTER, W_TO, W_THREE, W_AM};
  expectWords(2, 45, false, expected, 5);
}

void testTenTo() {
  // 12:50 PM → IT IS TEN MINUTES TO ONE PM
  int expected[] = {W_IT_IS, W_TEN_MIN, W_MINUTES, W_TO, W_ONE, W_PM};
  expectWords(12, 50, true, expected, 6);
}

void testFiveTo() {
  // 7:55 AM → IT IS FIVE MINUTES TO EIGHT AM
  int expected[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_TO, W_EIGHT, W_AM};
  expectWords(7, 55, false, expected, 6);
}

void testHourWrapAround() {
  // 12:55 PM → IT IS FIVE MINUTES TO ONE PM (wraps 12→1)
  int expected[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_TO, W_ONE, W_PM};
  expectWords(12, 55, true, expected, 6);

  // 12:35 AM → IT IS TWENTY FIVE MINUTES TO ONE AM
  int expected2[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_TO, W_ONE, W_AM};
  expectWords(12, 35, false, expected2, 7);
}

void testAllHoursAtOclock() {
  int hours[] = {W_ONE, W_TWO, W_THREE, W_FOUR, W_FIVE_HR, W_SIX,
                 W_SEVEN, W_EIGHT, W_NINE, W_TEN_HR, W_ELEVEN, W_TWELVE};
  for (int h = 1; h <= 12; h++) {
    int expected[] = {W_IT_IS, hours[h-1], W_OCLOCK, W_AM};
    expectWords(h, 0, false, expected, 4);
  }
}

int main() {
  testOclock();
  testFivePast();
  testTenPast();
  testQuarterPast();
  testTwentyPast();
  testTwentyFivePast();
  testHalfPast();
  testTwentyFiveTo();
  testTwentyTo();
  testQuarterTo();
  testTenTo();
  testFiveTo();
  testHourWrapAround();
  testAllHoursAtOclock();

  printf("\n%d/%d tests passed\n", tests_passed, tests_run);
  return (tests_passed == tests_run) ? 0 : 1;
}
```

- [ ] **Step 3: Create the Makefile**

```makefile
# test/Makefile
CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra

test: test_time_to_words
	./test_time_to_words

test_time_to_words: test_time_to_words.cpp ../word-clock/time_to_words.cpp ../word-clock/time_to_words.h ../word-clock/config.h
	$(CXX) $(CXXFLAGS) -o $@ test_time_to_words.cpp

clean:
	rm -f test_time_to_words

.PHONY: test clean
```

- [ ] **Step 4: Create time_to_words.cpp with minimal stub**

```cpp
// word-clock/time_to_words.cpp
#include "time_to_words.h"

static int hourWord(int hour) {
  switch (hour) {
    case 1:  return W_ONE;
    case 2:  return W_TWO;
    case 3:  return W_THREE;
    case 4:  return W_FOUR;
    case 5:  return W_FIVE_HR;
    case 6:  return W_SIX;
    case 7:  return W_SEVEN;
    case 8:  return W_EIGHT;
    case 9:  return W_NINE;
    case 10: return W_TEN_HR;
    case 11: return W_ELEVEN;
    case 12: return W_TWELVE;
    default: return W_TWELVE;
  }
}

static int nextHour(int hour) {
  return (hour % 12) + 1;
}

WordSet timeToWords(int hour, int minute, bool isPM) {
  WordSet ws;
  ws.count = 0;

  // IT IS is always on
  ws.words[ws.count++] = W_IT_IS;

  int fiveBlock = minute / 5;

  switch (fiveBlock) {
    case 0:  // :00-:04 → [hour] O'CLOCK
      ws.words[ws.count++] = hourWord(hour);
      ws.words[ws.count++] = W_OCLOCK;
      break;

    case 1:  // :05-:09 → FIVE MINUTES PAST [hour]
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 2:  // :10-:14 → TEN MINUTES PAST [hour]
      ws.words[ws.count++] = W_TEN_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 3:  // :15-:19 → QUARTER PAST [hour]
      ws.words[ws.count++] = W_QUARTER;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 4:  // :20-:24 → TWENTY MINUTES PAST [hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 5:  // :25-:29 → TWENTY FIVE MINUTES PAST [hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 6:  // :30-:34 → HALF PAST [hour]
      ws.words[ws.count++] = W_HALF;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 7:  // :35-:39 → TWENTY FIVE MINUTES TO [next hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 8:  // :40-:44 → TWENTY MINUTES TO [next hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 9:  // :45-:49 → QUARTER TO [next hour]
      ws.words[ws.count++] = W_QUARTER;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 10: // :50-:54 → TEN MINUTES TO [next hour]
      ws.words[ws.count++] = W_TEN_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 11: // :55-:59 → FIVE MINUTES TO [next hour]
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;
  }

  // AM or PM
  ws.words[ws.count++] = isPM ? W_PM : W_AM;

  return ws;
}
```

- [ ] **Step 5: Run tests to verify they pass**

Run: `cd /Users/samueldacanay/dev/personal/word-clock/test && make test`
Expected: All tests pass — `N/N tests passed`

- [ ] **Step 6: Verify Arduino compilation still works**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors

- [ ] **Step 7: Commit**

```bash
git add word-clock/time_to_words.h word-clock/time_to_words.cpp test/
git commit -m "feat: add time_to_words with native macOS tests

TDD-developed pure logic module. All 144 time-to-word mappings
tested natively without Arduino hardware."
```

---

## Task 3: display module — Turn Words On/Off

**Files:**
- Create: `word-clock/display.h`
- Create: `word-clock/display.cpp`

- [ ] **Step 1: Create display.h**

```cpp
// word-clock/display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "time_to_words.h"

// Initialize all word pins as OUTPUT and turn them off
void displayInit();

// Turn off all words
void displayClear();

// Show exactly the words in the given WordSet (turns off all others)
void displayShow(const WordSet& ws);

// Turn on a single word by index (for testing)
void displayWordOn(int word);

// Turn off a single word by index (for testing)
void displayWordOff(int word);

#endif
```

- [ ] **Step 2: Create display.cpp**

```cpp
// word-clock/display.cpp
#include "display.h"
#include <Arduino.h>

void displayInit() {
  for (int i = 0; i < NUM_WORDS; i++) {
    pinMode(WORD_PINS[i], OUTPUT);
    digitalWrite(WORD_PINS[i], LOW);
  }
}

void displayClear() {
  for (int i = 0; i < NUM_WORDS; i++) {
    digitalWrite(WORD_PINS[i], LOW);
  }
}

void displayShow(const WordSet& ws) {
  displayClear();
  for (int i = 0; i < ws.count; i++) {
    digitalWrite(WORD_PINS[ws.words[i]], HIGH);
  }
}

void displayWordOn(int word) {
  if (word >= 0 && word < NUM_WORDS) {
    digitalWrite(WORD_PINS[word], HIGH);
  }
}

void displayWordOff(int word) {
  if (word >= 0 && word < NUM_WORDS) {
    digitalWrite(WORD_PINS[word], LOW);
  }
}
```

- [ ] **Step 3: Verify Arduino compilation**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors

- [ ] **Step 4: Commit**

```bash
git add word-clock/display.h word-clock/display.cpp
git commit -m "feat: add display module for MOSFET pin control"
```

---

## Task 4: clock module — DS3231 RTC

**Files:**
- Create: `word-clock/clock.h`
- Create: `word-clock/clock.cpp`

- [ ] **Step 1: Install RTClib library**

Run: `arduino-cli lib install "RTClib"`
Expected: Library installed successfully

- [ ] **Step 2: Create clock.h**

```cpp
// word-clock/clock.h
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

struct ClockTime {
  int hour;    // 1-12
  int minute;  // 0-59
  bool isPM;
  int month;   // 1-12
  int day;     // 1-31
};

// Initialize the DS3231 RTC. Returns true if found.
bool clockInit();

// Read current time from RTC
ClockTime clockRead();

// Set time on RTC. hour24 is 0-23 format.
void clockSet(int hour24, int minute);

// Advance hour by 1 (wraps). Writes to RTC.
void clockAdvanceHour();

// Advance minute by 5 (wraps, increments hour at 60). Writes to RTC.
void clockAdvanceMinute5();

#endif
```

- [ ] **Step 3: Create clock.cpp**

```cpp
// word-clock/clock.cpp
#include "clock.h"
#include <Wire.h>
#include <RTClib.h>

static RTC_DS3231 rtc;

bool clockInit() {
  Wire.begin();
  return rtc.begin();
}

ClockTime clockRead() {
  DateTime now = rtc.now();
  ClockTime ct;
  int h24 = now.hour();
  ct.minute = now.minute();
  ct.month = now.month();
  ct.day = now.day();

  // Convert 24h to 12h
  ct.isPM = (h24 >= 12);
  ct.hour = h24 % 12;
  if (ct.hour == 0) ct.hour = 12;

  return ct;
}

void clockSet(int hour24, int minute) {
  DateTime now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour24, minute, 0));
}

void clockAdvanceHour() {
  DateTime now = rtc.now();
  int h = (now.hour() + 1) % 24;
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, now.minute(), now.second()));
}

void clockAdvanceMinute5() {
  DateTime now = rtc.now();
  int m = now.minute() + 5;
  int h = now.hour();
  if (m >= 60) {
    m -= 60;
    h = (h + 1) % 24;
  }
  // Round down to nearest 5
  m = (m / 5) * 5;
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, m, 0));
}
```

- [ ] **Step 4: Verify Arduino compilation**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors

- [ ] **Step 5: Commit**

```bash
git add word-clock/clock.h word-clock/clock.cpp
git commit -m "feat: add clock module for DS3231 RTC read/write"
```

---

## Task 5: buttons module — Debounce + Hold-Repeat

**Files:**
- Create: `word-clock/buttons.h`
- Create: `word-clock/buttons.cpp`

- [ ] **Step 1: Create buttons.h**

```cpp
// word-clock/buttons.h
#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"

// Initialize button pins with internal pullup
void buttonsInit();

// Call every loop iteration. Returns true if hour button was pressed/repeated.
bool buttonsHourPressed();

// Call every loop iteration. Returns true if minute button was pressed/repeated.
bool buttonsMinutePressed();

#endif
```

- [ ] **Step 2: Create buttons.cpp**

```cpp
// word-clock/buttons.cpp
#include "buttons.h"
#include <Arduino.h>

static const unsigned long DEBOUNCE_MS = 50;
static const unsigned long HOLD_DELAY_MS = 500;
static const unsigned long REPEAT_INTERVAL_MS = 250;  // 4 per second

struct ButtonState {
  int pin;
  bool lastReading;
  bool pressed;
  unsigned long lastDebounceTime;
  unsigned long pressStartTime;
  unsigned long lastRepeatTime;
};

static ButtonState hourBtn;
static ButtonState minBtn;

static void initButton(ButtonState& btn, int pin) {
  btn.pin = pin;
  btn.lastReading = HIGH;  // pullup = HIGH when not pressed
  btn.pressed = false;
  btn.lastDebounceTime = 0;
  btn.pressStartTime = 0;
  btn.lastRepeatTime = 0;
  pinMode(pin, INPUT_PULLUP);
}

static bool updateButton(ButtonState& btn) {
  bool reading = digitalRead(btn.pin);
  unsigned long now = millis();
  bool triggered = false;

  if (reading != btn.lastReading) {
    btn.lastDebounceTime = now;
  }

  if ((now - btn.lastDebounceTime) > DEBOUNCE_MS) {
    bool currentlyPressed = (reading == LOW);  // active low with pullup

    if (currentlyPressed && !btn.pressed) {
      // Fresh press
      triggered = true;
      btn.pressed = true;
      btn.pressStartTime = now;
      btn.lastRepeatTime = now;
    } else if (currentlyPressed && btn.pressed) {
      // Held down — check for repeat
      unsigned long held = now - btn.pressStartTime;
      if (held > HOLD_DELAY_MS && (now - btn.lastRepeatTime) > REPEAT_INTERVAL_MS) {
        triggered = true;
        btn.lastRepeatTime = now;
      }
    } else if (!currentlyPressed) {
      btn.pressed = false;
    }
  }

  btn.lastReading = reading;
  return triggered;
}

void buttonsInit() {
  initButton(hourBtn, PIN_BTN_HOUR);
  initButton(minBtn, PIN_BTN_MIN);
}

bool buttonsHourPressed() {
  return updateButton(hourBtn);
}

bool buttonsMinutePressed() {
  return updateButton(minBtn);
}
```

- [ ] **Step 3: Verify Arduino compilation**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors

- [ ] **Step 4: Commit**

```bash
git add word-clock/buttons.h word-clock/buttons.cpp
git commit -m "feat: add buttons module with debounce and hold-repeat"
```

---

## Task 6: birthday module — Date Check + Alternating Display

**Files:**
- Create: `word-clock/birthday.h`
- Create: `word-clock/birthday.cpp`

- [ ] **Step 1: Create birthday.h**

```cpp
// word-clock/birthday.h
#ifndef BIRTHDAY_H
#define BIRTHDAY_H

#include "config.h"
#include "time_to_words.h"
#include "clock.h"

// Returns true if today is the birthday
bool isBirthday(const ClockTime& ct);

// Returns the birthday WordSet (HAPPY BIRTH DAY CHELSEA + AM/PM)
WordSet birthdayWords(bool isPM);

// Given the time-based WordSet and clock, returns what to actually display.
// On birthday: alternates between time and birthday message every BIRTHDAY_CYCLE_MS.
// On other days: returns timeWords unchanged.
WordSet birthdayDisplay(const WordSet& timeWords, const ClockTime& ct);

#endif
```

- [ ] **Step 2: Create birthday.cpp**

```cpp
// word-clock/birthday.cpp
#include "birthday.h"
#include <Arduino.h>

bool isBirthday(const ClockTime& ct) {
  return (ct.month == BIRTHDAY_MONTH && ct.day == BIRTHDAY_DAY);
}

WordSet birthdayWords(bool isPM) {
  WordSet ws;
  ws.count = 0;
  ws.words[ws.count++] = W_IT_IS;
  ws.words[ws.count++] = W_HAPPY;
  ws.words[ws.count++] = W_BIRTH;
  ws.words[ws.count++] = W_DAY;
  ws.words[ws.count++] = W_CHELSEA;
  ws.words[ws.count++] = isPM ? W_PM : W_AM;
  return ws;
}

WordSet birthdayDisplay(const WordSet& timeWords, const ClockTime& ct) {
  if (!isBirthday(ct)) {
    return timeWords;
  }

  // Alternate between time and birthday message
  unsigned long phase = (millis() / BIRTHDAY_CYCLE_MS) % 2;
  if (phase == 0) {
    return timeWords;
  } else {
    return birthdayWords(ct.isPM);
  }
}
```

- [ ] **Step 3: Verify Arduino compilation**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors

- [ ] **Step 4: Commit**

```bash
git add word-clock/birthday.h word-clock/birthday.cpp
git commit -m "feat: add birthday module — alternating display on May 4th"
```

---

## Task 7: Main Sketch — Wire Everything Together

**Files:**
- Modify: `word-clock/word-clock.ino`

- [ ] **Step 1: Replace blink test with full sketch**

```cpp
// word-clock/word-clock.ino
#include "config.h"
#include "time_to_words.h"
#include "display.h"
#include "clock.h"
#include "buttons.h"
#include "birthday.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Word Clock starting...");

  displayInit();
  buttonsInit();

  if (!clockInit()) {
    Serial.println("ERROR: DS3231 RTC not found!");
    // Flash all words as error indicator
    for (int i = 0; i < NUM_WORDS; i++) {
      displayWordOn(i);
    }
    delay(1000);
    displayClear();
    delay(1000);
  }

  Serial.println("Word Clock ready.");
}

void loop() {
  // Handle button presses
  if (buttonsHourPressed()) {
    clockAdvanceHour();
    Serial.println("Hour advanced");
  }
  if (buttonsMinutePressed()) {
    clockAdvanceMinute5();
    Serial.println("Minute advanced by 5");
  }

  // Read current time
  ClockTime ct = clockRead();

  // Convert time to words
  WordSet ws = timeToWords(ct.hour, ct.minute, ct.isPM);

  // Apply birthday mode if applicable
  ws = birthdayDisplay(ws, ct);

  // Update display
  displayShow(ws);

  // Print time to serial for debugging
  static int lastMinute = -1;
  if (ct.minute != lastMinute) {
    lastMinute = ct.minute;
    Serial.print(ct.hour);
    Serial.print(":");
    if (ct.minute < 10) Serial.print("0");
    Serial.print(ct.minute);
    Serial.println(ct.isPM ? " PM" : " AM");
  }

  delay(100);  // 10Hz update rate
}
```

- [ ] **Step 2: Verify Arduino compilation**

Run: `arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/word-clock`
Expected: Compiles with no errors. Note memory usage.

- [ ] **Step 3: Commit**

```bash
git add word-clock/word-clock.ino
git commit -m "feat: main sketch wiring all modules together

Reads RTC, converts to words, handles buttons, applies birthday
mode, and drives display. Serial debug output for monitoring."
```

---

## Task 8: Test Sketches for Hardware Validation

**Files:**
- Create: `test-sketches/rtc-test/rtc-test.ino`
- Create: `test-sketches/pin-sweep/pin-sweep.ino`
- Create: `test-sketches/word-test/word-test.ino`
- Create: `test-sketches/button-test/button-test.ino`

- [ ] **Step 1: Create RTC test sketch**

```cpp
// test-sketches/rtc-test/rtc-test.ino
// Upload this first to verify the DS3231 RTC is working.
// Open Serial Monitor at 9600 baud. Should print time every second.

#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("ERROR: DS3231 not found! Check wiring: SDA=20, SCL=21");
    while (1) delay(1000);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power — setting to compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("RTC OK. Reading time every second:");
}

void loop() {
  DateTime now = rtc.now();
  Serial.print(now.hour());
  Serial.print(":");
  if (now.minute() < 10) Serial.print("0");
  Serial.print(now.minute());
  Serial.print(":");
  if (now.second() < 10) Serial.print("0");
  Serial.print(now.second());
  Serial.print("  ");
  Serial.print(now.month());
  Serial.print("/");
  Serial.print(now.day());
  Serial.print("/");
  Serial.println(now.year());
  delay(1000);
}
```

- [ ] **Step 2: Create pin sweep test sketch**

```cpp
// test-sketches/pin-sweep/pin-sweep.ino
// Cycles through each MOSFET output one at a time, 2 seconds each.
// Open Serial Monitor at 9600 baud to see which pin/word is active.
// Watch the clock face and note which word lights up for each pin.
// Use this to verify/correct the pin mapping in config.h.

// Copy these from config.h so this sketch is self-contained:
const char* WORD_NAMES[] = {
  "IT_IS", "TEN_MIN", "HALF", "QUARTER", "TWENTY", "FIVE_MIN",
  "MINUTES", "PAST", "TO", "ONE", "TWO", "THREE", "FOUR",
  "FIVE_HR", "SIX", "SEVEN", "EIGHT", "NINE", "TEN_HR",
  "ELEVEN", "TWELVE", "OCLOCK", "AM", "PM",
  "HAPPY", "BIRTH", "CHELSEA", "DAY"
};

const int WORD_PINS[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10,     // IT_IS..TO
  22, 26, 13, 24, 33, 30, 28, 27,  // ONE..EIGHT
  29, 34, 23, 31, 35, 36, 37,      // NINE..PM
  11, 12, 32, 25                    // HAPPY..DAY
};

const int NUM_WORDS = 28;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_WORDS; i++) {
    pinMode(WORD_PINS[i], OUTPUT);
    digitalWrite(WORD_PINS[i], LOW);
  }
  Serial.println("Pin Sweep Test — watch the clock face");
  Serial.println("Each pin lights up for 2 seconds.");
  Serial.println("Note which word lights up and compare to the expected name.");
  Serial.println("---");
}

void loop() {
  for (int i = 0; i < NUM_WORDS; i++) {
    Serial.print("Pin ");
    Serial.print(WORD_PINS[i]);
    Serial.print(" -> Expected: ");
    Serial.println(WORD_NAMES[i]);

    digitalWrite(WORD_PINS[i], HIGH);
    delay(2000);
    digitalWrite(WORD_PINS[i], LOW);
    delay(500);
  }
  Serial.println("--- Cycle complete. Starting over. ---");
}
```

- [ ] **Step 3: Create word test sketch**

```cpp
// test-sketches/word-test/word-test.ino
// Type a word name in Serial Monitor to light it up.
// Type "all" to light all words. Type "off" to turn all off.
// Type "time" to show current time in words.

#include <Wire.h>
#include <RTClib.h>

const char* WORD_NAMES[] = {
  "itis", "tenmin", "half", "quarter", "twenty", "fivemin",
  "minutes", "past", "to", "one", "two", "three", "four",
  "fivehr", "six", "seven", "eight", "nine", "tenhr",
  "eleven", "twelve", "oclock", "am", "pm",
  "happy", "birth", "chelsea", "day"
};

const int WORD_PINS[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10,     // IT_IS..TO
  22, 26, 13, 24, 33, 30, 28, 27,  // ONE..EIGHT
  29, 34, 23, 31, 35, 36, 37,      // NINE..PM
  11, 12, 32, 25                    // HAPPY..DAY
};

const int NUM_WORDS = 28;

void allOff() {
  for (int i = 0; i < NUM_WORDS; i++) {
    digitalWrite(WORD_PINS[i], LOW);
  }
}

void allOn() {
  for (int i = 0; i < NUM_WORDS; i++) {
    digitalWrite(WORD_PINS[i], HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_WORDS; i++) {
    pinMode(WORD_PINS[i], OUTPUT);
    digitalWrite(WORD_PINS[i], LOW);
  }
  Serial.println("Word Test — type a word name to light it up");
  Serial.println("Commands: all, off, or any word name (lowercase)");
  Serial.println("Words: itis tenmin half quarter twenty fivemin minutes");
  Serial.println("       past to one two three four fivehr six seven");
  Serial.println("       eight nine tenhr eleven twelve oclock am pm");
  Serial.println("       happy birth chelsea day");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "all") {
      allOn();
      Serial.println("All words ON");
    } else if (cmd == "off") {
      allOff();
      Serial.println("All words OFF");
    } else {
      bool found = false;
      for (int i = 0; i < NUM_WORDS; i++) {
        if (cmd == WORD_NAMES[i]) {
          digitalWrite(WORD_PINS[i], HIGH);
          Serial.print(WORD_NAMES[i]);
          Serial.println(" ON");
          found = true;
          break;
        }
      }
      if (!found) {
        Serial.print("Unknown: ");
        Serial.println(cmd);
      }
    }
  }
}
```

- [ ] **Step 4: Create button test sketch**

```cpp
// test-sketches/button-test/button-test.ino
// Tests button wiring. Press each button and watch Serial Monitor.
// Buttons use internal pullup — wire between pin and GND.

const int PIN_BTN_HOUR = 38;
const int PIN_BTN_MIN = 39;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN_HOUR, INPUT_PULLUP);
  pinMode(PIN_BTN_MIN, INPUT_PULLUP);
  Serial.println("Button Test — press buttons and watch output");
  Serial.println("Hour button: pin 38");
  Serial.println("Minute button: pin 39");
}

void loop() {
  static bool lastHour = HIGH;
  static bool lastMin = HIGH;

  bool hour = digitalRead(PIN_BTN_HOUR);
  bool min = digitalRead(PIN_BTN_MIN);

  if (hour != lastHour) {
    Serial.print("Hour button: ");
    Serial.println(hour == LOW ? "PRESSED" : "RELEASED");
    lastHour = hour;
  }

  if (min != lastMin) {
    Serial.print("Minute button: ");
    Serial.println(min == LOW ? "PRESSED" : "RELEASED");
    lastMin = min;
  }

  delay(10);
}
```

- [ ] **Step 5: Verify all test sketches compile**

Run:
```bash
arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/test-sketches/rtc-test
arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/test-sketches/pin-sweep
arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/test-sketches/word-test
arduino-cli compile --fqbn arduino:avr:mega /Users/samueldacanay/dev/personal/word-clock/test-sketches/button-test
```
Expected: All compile with no errors

- [ ] **Step 6: Commit**

```bash
git add test-sketches/
git commit -m "feat: add hardware test sketches for incremental validation

rtc-test: verify DS3231 reads time
pin-sweep: cycle each output to map words to pins
word-test: serial command to light specific words
button-test: verify button wiring"
```

---

## Task 9: Push to GitHub

- [ ] **Step 1: Push all commits**

Run: `git push -u origin main`
Expected: All commits pushed to dakaneye/word-clock

---

## Hardware Testing Order (after parts arrive)

This is not code — it's the testing procedure to follow once assembly is complete:

1. **RTC test** — Upload `test-sketches/rtc-test/rtc-test.ino` to Mega. Open Serial Monitor. Verify time prints every second. If RTC not found, check SDA/SCL wiring (pins 20/21).

2. **Pin sweep** — Wire one MOSFET module at a time. Upload `test-sketches/pin-sweep/pin-sweep.ino`. Watch which word lights for each pin. Update `config.h` pin numbers if any are wrong.

3. **Word test** — After all modules wired, upload `test-sketches/word-test/word-test.ino`. Type word names to verify each lights the correct word.

4. **Button test** — Wire buttons (pin to GND). Upload `test-sketches/button-test/button-test.ino`. Press each button, verify Serial output.

5. **Full system** — Upload the main `word-clock/word-clock.ino`. Set time with buttons. Verify display is correct.
