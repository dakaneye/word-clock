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
