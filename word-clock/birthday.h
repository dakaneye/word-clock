// word-clock/birthday.h
#ifndef BIRTHDAY_H
#define BIRTHDAY_H

#include "config.h"
#include "time_to_words.h"
#include "clock.h"

// Returns true if today is the birthday
bool isBirthday(const ClockTime& ct);

// Returns the birthday WordSet (IT IS HAPPY BIRTH DAY CHELSEA + AM/PM)
WordSet birthdayWords(bool isPM);

// Given the time-based WordSet and clock, returns what to actually display.
// On birthday: alternates between time and birthday message every BIRTHDAY_CYCLE_MS.
// On other days: returns timeWords unchanged.
WordSet birthdayDisplay(const WordSet& timeWords, const ClockTime& ct);

#endif
