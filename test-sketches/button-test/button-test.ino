// test-sketches/button-test/button-test.ino
// Tests button wiring. Press each button and watch Serial Monitor.
// Buttons use internal pullup — wire between pin and GND.

const int PIN_BTN_HOUR = 38;
const int PIN_BTN_MIN = 39;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN_HOUR, INPUT_PULLUP);
  pinMode(PIN_BTN_MIN, INPUT_PULLUP);
  Serial.println("Button Test — press buttons and watch output");
  Serial.println("Hour button: pin 38");
  Serial.println("Minute button: pin 39");
}

void loop() {
  static bool lastHour = HIGH;
  static bool lastMin = HIGH;

  bool hour = digitalRead(PIN_BTN_HOUR);
  bool min = digitalRead(PIN_BTN_MIN);

  if (hour != lastHour) {
    Serial.print("Hour button: ");
    Serial.println(hour == LOW ? "PRESSED" : "RELEASED");
    lastHour = hour;
  }

  if (min != lastMin) {
    Serial.print("Minute button: ");
    Serial.println(min == LOW ? "PRESSED" : "RELEASED");
    lastMin = min;
  }

  delay(10);
}
