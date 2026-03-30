// word-clock/display.cpp
#include "display.h"
#include <Arduino.h>

void displayInit() {
  for (int i = 0; i < NUM_WORDS; i++) {
    pinMode(WORD_PINS[i], OUTPUT);
    digitalWrite(WORD_PINS[i], LOW);
  }
}

void displayClear() {
  for (int i = 0; i < NUM_WORDS; i++) {
    digitalWrite(WORD_PINS[i], LOW);
  }
}

void displayShow(const WordSet& ws) {
  displayClear();
  for (int i = 0; i < ws.count; i++) {
    int word = ws.words[i];
    if (word >= 0 && word < NUM_WORDS) {
      digitalWrite(WORD_PINS[word], HIGH);
    }
  }
}

void displayWordOn(int word) {
  if (word >= 0 && word < NUM_WORDS) {
    digitalWrite(WORD_PINS[word], HIGH);
  }
}
