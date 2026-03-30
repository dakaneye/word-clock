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
