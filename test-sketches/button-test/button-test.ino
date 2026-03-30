// test-sketches/button-test/button-test.ino
// Tests button wiring. Press each button and watch Serial Monitor.
// Buttons use internal pullup — wire between pin and GND.
//
// Button pins are duplicated here because Arduino IDE copies sketches
// to a temp directory before compiling, breaking relative #include paths.
// If you change button pins in config.h, update this file too.

const int PIN_BTN_HOUR = 38;
const int PIN_BTN_MIN = 39;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BTN_HOUR, INPUT_PULLUP);
  pinMode(PIN_BTN_MIN, INPUT_PULLUP);
  Serial.println("Button Test — press buttons and watch output");
  Serial.print("Hour button: pin ");
  Serial.println(PIN_BTN_HOUR);
  Serial.print("Minute button: pin ");
  Serial.println(PIN_BTN_MIN);
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
