// test-sketches/word-test/word-test.ino
// Type a word name in Serial Monitor to light it up.
// Type "all" to light all words. Type "off" to turn all off.

const char* WORD_NAMES[] = {
  "itis", "tenmin", "half", "quarter", "twenty", "fivemin",
  "minutes", "past", "to", "one", "two", "three", "four",
  "fivehr", "six", "seven", "eight", "nine", "tenhr",
  "eleven", "twelve", "oclock", "am", "pm",
  "happy", "birth", "chelsea", "day"
};

const int WORD_PINS[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10,     // IT_IS..TO
  22, 26, 13, 24, 33, 30, 28, 27,  // ONE..EIGHT
  29, 34, 23, 31, 35, 36, 37,      // NINE..PM
  11, 12, 32, 25                    // HAPPY..DAY
};

const int NUM_WORDS = 28;

void allOff() {
  for (int i = 0; i < NUM_WORDS; i++) {
    digitalWrite(WORD_PINS[i], LOW);
  }
}

void allOn() {
  for (int i = 0; i < NUM_WORDS; i++) {
    digitalWrite(WORD_PINS[i], HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_WORDS; i++) {
    pinMode(WORD_PINS[i], OUTPUT);
    digitalWrite(WORD_PINS[i], LOW);
  }
  Serial.println("Word Test — type a word name to light it up");
  Serial.println("Commands: all, off, or any word name (lowercase)");
  Serial.println("Words: itis tenmin half quarter twenty fivemin minutes");
  Serial.println("       past to one two three four fivehr six seven");
  Serial.println("       eight nine tenhr eleven twelve oclock am pm");
  Serial.println("       happy birth chelsea day");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "all") {
      allOn();
      Serial.println("All words ON");
    } else if (cmd == "off") {
      allOff();
      Serial.println("All words OFF");
    } else {
      bool found = false;
      for (int i = 0; i < NUM_WORDS; i++) {
        if (cmd == WORD_NAMES[i]) {
          digitalWrite(WORD_PINS[i], HIGH);
          Serial.print(WORD_NAMES[i]);
          Serial.println(" ON");
          found = true;
          break;
        }
      }
      if (!found) {
        Serial.print("Unknown: ");
        Serial.println(cmd);
      }
    }
  }
}
