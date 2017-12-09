#define PIN_PUMP 8

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
}

boolean test;

void loop() {
  digitalWrite(LED_BUILTIN, test);
  digitalWrite(PIN_PUMP, test);
  test = !test;
  delay(5000);
}
