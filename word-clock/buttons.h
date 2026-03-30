// word-clock/buttons.h
#ifndef BUTTONS_H
#define BUTTONS_H

#include "config.h"

// Initialize button pins with internal pullup
void buttonsInit();

// Call every loop iteration. Returns true if hour button was pressed/repeated.
bool buttonsHourPressed();

// Call every loop iteration. Returns true if minute button was pressed/repeated.
bool buttonsMinutePressed();

#endif
