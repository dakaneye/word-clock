// Word Clock - Blink test
// Verifies Arduino is working and uploads succeed.
// The onboard LED (pin 13) blinks once per second.

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Word Clock - blink test OK");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
