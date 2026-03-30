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
