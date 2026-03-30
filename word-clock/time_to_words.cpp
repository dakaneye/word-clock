// word-clock/time_to_words.cpp
#include "time_to_words.h"

static int hourWord(int hour) {
  switch (hour) {
    case 1:  return W_ONE;
    case 2:  return W_TWO;
    case 3:  return W_THREE;
    case 4:  return W_FOUR;
    case 5:  return W_FIVE_HR;
    case 6:  return W_SIX;
    case 7:  return W_SEVEN;
    case 8:  return W_EIGHT;
    case 9:  return W_NINE;
    case 10: return W_TEN_HR;
    case 11: return W_ELEVEN;
    case 12: return W_TWELVE;
    default: return W_TWELVE;
  }
}

static int nextHour(int hour) {
  return (hour % 12) + 1;
}

WordSet timeToWords(int hour, int minute, bool isPM) {
  WordSet ws;
  ws.count = 0;

  // IT IS is always on
  ws.words[ws.count++] = W_IT_IS;

  int fiveBlock = minute / 5;

  switch (fiveBlock) {
    case 0:  // :00-:04 -> [hour] O'CLOCK
      ws.words[ws.count++] = hourWord(hour);
      ws.words[ws.count++] = W_OCLOCK;
      break;

    case 1:  // :05-:09 -> FIVE MINUTES PAST [hour]
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 2:  // :10-:14 -> TEN MINUTES PAST [hour]
      ws.words[ws.count++] = W_TEN_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 3:  // :15-:19 -> QUARTER PAST [hour]
      ws.words[ws.count++] = W_QUARTER;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 4:  // :20-:24 -> TWENTY MINUTES PAST [hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 5:  // :25-:29 -> TWENTY FIVE MINUTES PAST [hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 6:  // :30-:34 -> HALF PAST [hour]
      ws.words[ws.count++] = W_HALF;
      ws.words[ws.count++] = W_PAST;
      ws.words[ws.count++] = hourWord(hour);
      break;

    case 7:  // :35-:39 -> TWENTY FIVE MINUTES TO [next hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 8:  // :40-:44 -> TWENTY MINUTES TO [next hour]
      ws.words[ws.count++] = W_TWENTY;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 9:  // :45-:49 -> QUARTER TO [next hour]
      ws.words[ws.count++] = W_QUARTER;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 10: // :50-:54 -> TEN MINUTES TO [next hour]
      ws.words[ws.count++] = W_TEN_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;

    case 11: // :55-:59 -> FIVE MINUTES TO [next hour]
      ws.words[ws.count++] = W_FIVE_MIN;
      ws.words[ws.count++] = W_MINUTES;
      ws.words[ws.count++] = W_TO;
      ws.words[ws.count++] = hourWord(nextHour(hour));
      break;
  }

  // AM or PM
  ws.words[ws.count++] = isPM ? W_PM : W_AM;

  return ws;
}
