// word-clock/display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "time_to_words.h"

// Initialize all word pins as OUTPUT and turn them off
void displayInit();

// Turn off all words
void displayClear();

// Show exactly the words in the given WordSet (turns off all others)
void displayShow(const WordSet& ws);

// Turn on a single word by index (for testing)
void displayWordOn(int word);

// Turn off a single word by index (for testing)
void displayWordOff(int word);

#endif
