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
