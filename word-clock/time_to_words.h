// word-clock/time_to_words.h
#ifndef TIME_TO_WORDS_H
#define TIME_TO_WORDS_H

#include "config.h"

// Maximum words that can be active at once
// IT IS + TWENTY + FIVE + MINUTES + PAST + hour + AM/PM = 7
const int MAX_ACTIVE_WORDS = 8;

struct WordSet {
  int words[MAX_ACTIVE_WORDS];
  int count;
};

// Given hour (1-12), minute (0-59), and isPM flag,
// returns which words should be lit.
// Hour must be 1-12 (not 0-23).
WordSet timeToWords(int hour, int minute, bool isPM);

#endif
