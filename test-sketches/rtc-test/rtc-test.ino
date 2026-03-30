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
