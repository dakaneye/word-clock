// test-sketches/pin-sweep/pin-sweep.ino
// Cycles through each MOSFET output one at a time, 2 seconds each.
// Open Serial Monitor at 9600 baud to see which pin/word is active.
// Watch the clock face and note which word lights up for each pin.
// Use this to verify/correct the pin mapping in config.h.

// Copy these from config.h so this sketch is self-contained:
const char* WORD_NAMES[] = {
  "IT_IS", "TEN_MIN", "HALF", "QUARTER", "TWENTY", "FIVE_MIN",
  "MINUTES", "PAST", "TO", "ONE", "TWO", "THREE", "FOUR",
  "FIVE_HR", "SIX", "SEVEN", "EIGHT", "NINE", "TEN_HR",
  "ELEVEN", "TWELVE", "OCLOCK", "AM", "PM",
  "HAPPY", "BIRTH", "CHELSEA", "DAY"
};

const int WORD_PINS[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10,     // IT_IS..TO
  22, 26, 13, 24, 33, 30, 28, 27,  // ONE..EIGHT
  29, 34, 23, 31, 35, 36, 37,      // NINE..PM
  11, 12, 32, 25                    // HAPPY..DAY
};

const int NUM_WORDS = 28;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_WORDS; i++) {
    pinMode(WORD_PINS[i], OUTPUT);
    digitalWrite(WORD_PINS[i], LOW);
  }
  Serial.println("Pin Sweep Test — watch the clock face");
  Serial.println("Each pin lights up for 2 seconds.");
  Serial.println("Note which word lights up and compare to the expected name.");
  Serial.println("---");
}

void loop() {
  for (int i = 0; i < NUM_WORDS; i++) {
    Serial.print("Pin ");
    Serial.print(WORD_PINS[i]);
    Serial.print(" -> Expected: ");
    Serial.println(WORD_NAMES[i]);

    digitalWrite(WORD_PINS[i], HIGH);
    delay(2000);
    digitalWrite(WORD_PINS[i], LOW);
    delay(500);
  }
  Serial.println("--- Cycle complete. Starting over. ---");
}
