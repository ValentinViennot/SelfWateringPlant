#define PIN_DO 7
#define PIN_AO A0

void setup() {
  //pinMode(PIN_DO, INPUT);
  Serial.begin(9600);//bauds
}

void loop() {
  //Serial.print("digital ");
  //Serial.println(digitalRead(PIN_DO));
  Serial.print("analog ");
  Serial.println(analogRead(PIN_AO));
  delay(1500);
}
