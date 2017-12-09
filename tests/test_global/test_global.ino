#include <JeeLib.h>
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog

#define PIN_LED_GREEN 7
#define PIN_LED_RED 6
#define PIN_LED_BLUE 5
#define PIN_PUMP 8
#define PIN_BTN 4
#define PIN_SENSOR_HYG A0

void setup() {
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BTN, INPUT);
  Sleepy::loseSomeTime(1000);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Starting test sequence...");
  delay(5000);
  Serial.println("BLUE LED 1 second");
  digitalWrite(PIN_LED_BLUE, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_BLUE, LOW);
  //delay(1000);
  Serial.println("GREEN LED 1 second");
  digitalWrite(PIN_LED_GREEN, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_GREEN, LOW);
  //delay(1000);
  Serial.println("RED LED 1 second");
  digitalWrite(PIN_LED_RED, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_RED, LOW);
  delay(1000);
  Serial.println("10 measurements hygro sensor (5s) :");
  for (int i = 0 ; i < 10 ; ++i) {
     Serial.println(analogRead(PIN_SENSOR_HYG));
     delay(500);
  }
  Serial.println("10 measurements button (5s) :");
  for (int i = 0 ; i < 10 ; ++i) {
     Serial.println(digitalRead(PIN_BTN));
     delay(500);
  }
  delay(1000);
  Serial.println("Using pump for 3seconds");
  digitalWrite(PIN_PUMP, HIGH);
  delay(3000);
  digitalWrite(PIN_PUMP, LOW);
  delay(100);
  Serial.println("Sleeping for 10 seconds");
  delay(100);
  Sleepy::loseSomeTime(10000);
}
