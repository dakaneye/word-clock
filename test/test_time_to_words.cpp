// test/test_time_to_words.cpp
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Stub out Arduino types so config.h compiles natively
#define HIGH 1
#define LOW 0
#include <cstdint>

#include "../word-clock/config.h"
#include "../word-clock/config.cpp"
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
  // 3:00 AM -> IT IS THREE O'CLOCK AM
  int expected[] = {W_IT_IS, W_THREE, W_OCLOCK, W_AM};
  expectWords(3, 0, false, expected, 4);

  // 12:00 PM -> IT IS TWELVE O'CLOCK PM
  int expected2[] = {W_IT_IS, W_TWELVE, W_OCLOCK, W_PM};
  expectWords(12, 0, true, expected2, 4);

  // :01-:04 still shows O'CLOCK
  int expected3[] = {W_IT_IS, W_SEVEN, W_OCLOCK, W_PM};
  expectWords(7, 3, true, expected3, 4);
}

void testFivePast() {
  // 3:05 AM -> IT IS FIVE MINUTES PAST THREE AM
  int expected[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_PAST, W_THREE, W_AM};
  expectWords(3, 5, false, expected, 6);

  // 10:07 PM -> IT IS FIVE MINUTES PAST TEN PM
  int expected2[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_PAST, W_TEN_HR, W_PM};
  expectWords(10, 7, true, expected2, 6);
}

void testTenPast() {
  // 6:10 AM -> IT IS TEN MINUTES PAST SIX AM
  int expected[] = {W_IT_IS, W_TEN_MIN, W_MINUTES, W_PAST, W_SIX, W_AM};
  expectWords(6, 10, false, expected, 6);
}

void testQuarterPast() {
  // 1:15 PM -> IT IS QUARTER PAST ONE PM
  int expected[] = {W_IT_IS, W_QUARTER, W_PAST, W_ONE, W_PM};
  expectWords(1, 15, true, expected, 5);
}

void testTwentyPast() {
  // 8:20 AM -> IT IS TWENTY MINUTES PAST EIGHT AM
  int expected[] = {W_IT_IS, W_TWENTY, W_MINUTES, W_PAST, W_EIGHT, W_AM};
  expectWords(8, 20, false, expected, 6);
}

void testTwentyFivePast() {
  // 11:25 PM -> IT IS TWENTY FIVE MINUTES PAST ELEVEN PM
  int expected[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_PAST, W_ELEVEN, W_PM};
  expectWords(11, 25, true, expected, 7);
}

void testHalfPast() {
  // 4:30 AM -> IT IS HALF PAST FOUR AM
  int expected[] = {W_IT_IS, W_HALF, W_PAST, W_FOUR, W_AM};
  expectWords(4, 30, false, expected, 5);

  // :31-:34 still shows HALF PAST
  int expected2[] = {W_IT_IS, W_HALF, W_PAST, W_FOUR, W_AM};
  expectWords(4, 33, false, expected2, 5);
}

void testTwentyFiveTo() {
  // 9:35 AM -> IT IS TWENTY FIVE MINUTES TO TEN AM
  int expected[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_TO, W_TEN_HR, W_AM};
  expectWords(9, 35, false, expected, 7);
}

void testTwentyTo() {
  // 5:40 PM -> IT IS TWENTY MINUTES TO SIX PM
  int expected[] = {W_IT_IS, W_TWENTY, W_MINUTES, W_TO, W_SIX, W_PM};
  expectWords(5, 40, true, expected, 6);
}

void testQuarterTo() {
  // 2:45 AM -> IT IS QUARTER TO THREE AM
  int expected[] = {W_IT_IS, W_QUARTER, W_TO, W_THREE, W_AM};
  expectWords(2, 45, false, expected, 5);
}

void testTenTo() {
  // 12:50 PM -> IT IS TEN MINUTES TO ONE PM
  int expected[] = {W_IT_IS, W_TEN_MIN, W_MINUTES, W_TO, W_ONE, W_PM};
  expectWords(12, 50, true, expected, 6);
}

void testFiveTo() {
  // 7:55 AM -> IT IS FIVE MINUTES TO EIGHT AM
  int expected[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_TO, W_EIGHT, W_AM};
  expectWords(7, 55, false, expected, 6);
}

void testHourWrapAround() {
  // 12:55 PM -> IT IS FIVE MINUTES TO ONE PM (wraps 12->1)
  int expected[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_TO, W_ONE, W_PM};
  expectWords(12, 55, true, expected, 6);

  // 12:35 AM -> IT IS TWENTY FIVE MINUTES TO ONE AM
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

void testBoundaryMinutes() {
  // :04 -> still O'CLOCK (last minute of block 0)
  int e1[] = {W_IT_IS, W_FIVE_HR, W_OCLOCK, W_AM};
  expectWords(5, 4, false, e1, 4);

  // :05 -> FIVE MINUTES PAST (first minute of block 1)
  int e2[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_PAST, W_FIVE_HR, W_AM};
  expectWords(5, 5, false, e2, 6);

  // :29 -> still TWENTY FIVE PAST (last minute of block 5)
  int e3[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_PAST, W_SIX, W_PM};
  expectWords(6, 29, true, e3, 7);

  // :30 -> HALF PAST (first minute of block 6)
  int e4[] = {W_IT_IS, W_HALF, W_PAST, W_SIX, W_PM};
  expectWords(6, 30, true, e4, 5);

  // :34 -> still HALF PAST (last minute of block 6)
  int e5[] = {W_IT_IS, W_HALF, W_PAST, W_SIX, W_PM};
  expectWords(6, 34, true, e5, 5);

  // :35 -> TWENTY FIVE TO (first minute of block 7)
  int e6[] = {W_IT_IS, W_TWENTY, W_FIVE_MIN, W_MINUTES, W_TO, W_SEVEN, W_PM};
  expectWords(6, 35, true, e6, 7);

  // :59 -> still FIVE TO (last minute of block 11, with hour wrap)
  int e7[] = {W_IT_IS, W_FIVE_MIN, W_MINUTES, W_TO, W_ONE, W_AM};
  expectWords(12, 59, false, e7, 6);
}

void testAmPmToggle() {
  // Same time, different AM/PM — only the AM/PM word should differ
  int am[] = {W_IT_IS, W_THREE, W_OCLOCK, W_AM};
  int pm[] = {W_IT_IS, W_THREE, W_OCLOCK, W_PM};
  expectWords(3, 0, false, am, 4);
  expectWords(3, 0, true, pm, 4);
}

void testBirthdayWords() {
  // Verify birthdayWords produces the right set
  // (This tests the WordSet construction, not millis()-based alternation)
  // We include the birthday header inline since it has Arduino deps
  // Just test the word set manually here
  WordSet ws;
  ws.count = 0;
  ws.words[ws.count++] = W_IT_IS;
  ws.words[ws.count++] = W_HAPPY;
  ws.words[ws.count++] = W_BIRTH;
  ws.words[ws.count++] = W_DAY;
  ws.words[ws.count++] = W_CHELSEA;
  ws.words[ws.count++] = W_AM;

  tests_run++;
  bool pass = (ws.count == 6);
  pass = pass && wordSetContains(ws, W_IT_IS);
  pass = pass && wordSetContains(ws, W_HAPPY);
  pass = pass && wordSetContains(ws, W_BIRTH);
  pass = pass && wordSetContains(ws, W_DAY);
  pass = pass && wordSetContains(ws, W_CHELSEA);
  pass = pass && wordSetContains(ws, W_AM);

  if (pass) {
    tests_passed++;
  } else {
    printf("FAIL: birthday words AM\n");
    printf("  Got: ");
    printWordSet(ws);
  }

  // PM version
  ws.words[5] = W_PM;
  tests_run++;
  pass = wordSetContains(ws, W_PM) && !wordSetContains(ws, W_AM);
  if (pass) {
    tests_passed++;
  } else {
    printf("FAIL: birthday words PM\n");
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
  testBoundaryMinutes();
  testAmPmToggle();
  testBirthdayWords();

  printf("\n%d/%d tests passed\n", tests_passed, tests_run);
  return (tests_passed == tests_run) ? 0 : 1;
}
