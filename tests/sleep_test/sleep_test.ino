#include <JeeLib.h>
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

boolean led = true; 
void loop() {
  digitalWrite(LED_BUILTIN, led);
  delay(500);
  Sleepy::loseSomeTime(10000);
  led = !led;
}
