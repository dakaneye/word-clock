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
    Serial.println("ERROR: DS3231 RTC not found! Check wiring: SDA=20, SCL=21");
    // Halt with infinite blink — do not proceed with garbage time data
    while (true) {
      for (int i = 0; i < NUM_WORDS; i++) displayWordOn(i);
      delay(500);
      displayClear();
      delay(500);
    }
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
