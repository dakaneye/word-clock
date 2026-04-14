// word-clock/clock.cpp
// Timekeeping using millis() — no RTC dependency.
// Accurate to ~±4 seconds/day via the Arduino's crystal.
// Time resets to 12:00 AM on power loss — set with buttons.
// Replace with RTC-based implementation when a working DS3231 is available.

#include "clock.h"
#include <Arduino.h>

static unsigned long baseMillis = 0;
static long baseSeconds = 0;  // seconds since midnight

static long currentSeconds() {
  unsigned long elapsed = millis() - baseMillis;
  return (baseSeconds + (long)(elapsed / 1000)) % 86400L;
}

bool clockInit() {
  baseMillis = millis();
  baseSeconds = 0;  // midnight — user sets with buttons
  return true;
}

ClockTime clockRead() {
  long secs = currentSeconds();
  int h24 = (int)(secs / 3600);
  int m = (int)((secs % 3600) / 60);
  int s = (int)(secs % 60);

  ClockTime ct;
  ct.hour = h24 % 12;
  if (ct.hour == 0) ct.hour = 12;
  ct.minute = m;
  ct.second = s;
  ct.isPM = (h24 >= 12);
  ct.month = 1;  // non-birthday month — birthday mode disabled without RTC
  ct.day = 1;
  return ct;
}

void clockSet(int hour24, int minute) {
  baseSeconds = (long)hour24 * 3600 + (long)minute * 60;
  baseMillis = millis();
}

void clockAdvanceHour() {
  long secs = currentSeconds();
  baseSeconds = (secs + 3600) % 86400L;
  baseMillis = millis();
}

void clockAdvanceMinute5() {
  long secs = currentSeconds();
  int m = (int)((secs % 3600) / 60);
  int h = (int)(secs / 3600);
  m = m + 5;
  if (m >= 60) {
    m -= 60;
    h = (h + 1) % 24;
  }
  m = (m / 5) * 5;
  baseSeconds = (long)h * 3600 + (long)m * 60;
  baseMillis = millis();
}
