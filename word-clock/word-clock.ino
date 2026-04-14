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
    Serial.println("ERROR: Clock init failed");
    while (true) {
      for (int i = 0; i < NUM_WORDS; i++) displayWordOn(i);
      delay(500);
      displayClear();
      delay(500);
    }
  }

  // Time sync: wait briefly for a "T<unix_seconds>" line on serial.
  // Host tool (make set-time) sends this right after opening the port.
  // Keep listening until either a valid sync completes or the window elapses —
  // don't exit on a stray blank line.
  Serial.println("READY");
  unsigned long syncDeadline = millis() + 3000;
  String line;
  bool synced = false;
  while (!synced && millis() < syncDeadline) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') {
        if (line.length() > 1 && line.charAt(0) == 'T') {
          uint32_t epoch = (uint32_t)line.substring(1).toInt();
          if (epoch > 0) {
            clockSetEpoch(epoch);
            Serial.print("Time synced to epoch ");
            Serial.println(epoch);
            synced = true;
          }
        }
        line = "";
      } else if (c != '\r') {
        line += c;
      }
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
