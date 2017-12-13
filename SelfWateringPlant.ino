#include <EEPROM.h>
#include <JeeLib.h>
ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog

#define PIN_LED_GREEN 7
#define PIN_LED_RED 6
#define PIN_LED_BLUE 5
#define PIN_PUMP 8
#define PIN_BTN 4
#define PIN_SENSOR_HYG A0

#define ADDR_MIN 0
#define ADDR_MAX (sizeof(int))

enum ProgramState {
  SLEEP,
  WATERING,
  CONTROL,
  CONFIG,
  CONFIG_LOW,
  CONFIG_HIGH,
};
ProgramState state;

// Number of values needed to calculate an average measure
const unsigned int NB_MSRT = 3;
// Levels
int MIN_WATER = 0, MAX_WATER = 0, water = 0;

// Last time a watering period started (ms)
unsigned long last_watering_start = 0;
// Watering period duration (ms)
const unsigned long T_WATERING = 1500;//ms

// Manage buttons (is pressed ? , is it the first press ?) and led blinking 
boolean btn = false, first = false, led_blink = false;

void setup() {
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BTN, INPUT);
  // Get MIN and MAX water values from EEPROM
  EEPROM.get(ADDR_MIN,MIN_WATER);
  EEPROM.get(ADDR_MAX,MAX_WATER);
  // DEBUG
  Serial.begin(9600);
  //
  // if MIN and MAX are already defined
  if (MAX_WATER > 0 && MIN_WATER > MAX_WATER) {
    Serial.print("MIN: ");
    Serial.println(MIN_WATER);
    Serial.print("MAX: ");
    Serial.println(MAX_WATER);
    state = CONTROL;
  }
  else // Else, we begin with configuration step
    state = CONFIG;
}

void loop() {
  switch (state) {
    case SLEEP:
      Serial.println("Sleep or config ?");
      if (digitalRead(PIN_BTN)) {
        state = CONFIG;
      } else {
        Serial.println("Sleeping for 10 seconds");
        delay(100);
        Sleepy::loseSomeTime(10000);
        state = CONTROL;
      }
      break;
    case WATERING:
      watering();
      delay(500);
    case CONTROL:
      control();
      break;
    case CONFIG:
      Serial.println("Entering configuration...");
      initConfigLoop();
      state = CONFIG_LOW;
      break;
    case CONFIG_LOW:
      if (first) {
        measure();
        MIN_WATER = water;
        Serial.print("MIN : ");
        Serial.println(MIN_WATER);
        EEPROM.put(ADDR_MIN,MIN_WATER);
        digitalWrite(PIN_LED_RED, LOW);
        digitalWrite(PIN_LED_BLUE, HIGH);
        delay(3000);
        initConfigLoop();
        state = CONFIG_HIGH;
      }
      else {
        configLoop(PIN_LED_RED);
      }
      break;
    case CONFIG_HIGH:
      if (first) {
        watering(false);
        measure();
        MAX_WATER = water;
        Serial.print("MAX : ");
        Serial.println(MAX_WATER);
        EEPROM.put(ADDR_MAX,MAX_WATER);
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_BLUE, HIGH);
        delay(3000);
        initConfigLoop();
        // The max level has to be inferor to min level (min is the hightest value and max the lowest)
        if (MAX_WATER >= MIN_WATER) 
          state = CONFIG;
        else // if config is successful, we start the controlling process 
          state = CONTROL;
      }
      else {
        configLoop(PIN_LED_GREEN);
        // Execute the watering process to test it and to calculate MAX level
        watering();
      }
      break;
  }
}

void initConfigLoop() {
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_BLUE, LOW);
  first = false;
  btn = false;
}

void configLoop(int pin) {
  led_blink = !led_blink;
  digitalWrite(pin, led_blink);
  delay(1000);
  if (digitalRead(PIN_BTN)) {
    first = !btn;
    btn = true;
  } else {
    btn = false;
  }
}

/**
 * Activating pump and blue led
 */
void watering() {
  watering(true);
}

/**
 * @param need Turn on (true) or off (false) watering process
 * Watering process : Water for T_WATERING ms , then wait for 4*T_WATERING ms (loop)
 */
void watering(boolean need) {
  if (need) {
    if ( millis() - last_watering_start > 5*T_WATERING ) {
      last_watering_start = millis();
      digitalWrite(PIN_PUMP, true);
    } else if ( millis() - last_watering_start > T_WATERING ) {
      digitalWrite(PIN_PUMP, false);
    }
  } else {
    digitalWrite(PIN_PUMP, false);
  }
  digitalWrite(PIN_LED_BLUE, need);
}

/**
 * Control water level
 * If water level is "under" MIN level, turn on red light and start watering process
 * If water level is "above" MAX level, turn on green light and go to sleep
 * If water level is between MIN and MAX, go to sleep
 */
void control() {
  measure();
  if (water >= MIN_WATER) {  
    // Plant needs watering
    state = WATERING;
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_GREEN, LOW);
  } else if (water <= MAX_WATER) {
    // Plant has enough water
    watering(false);
    state = SLEEP;
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_GREEN, HIGH);
    // wait to see the green light
    delay(3000);
    digitalWrite(PIN_LED_GREEN, LOW);    
  } else if (state != WATERING) {
    // if we're between MIN and MAX, no action required, go to sleep
    state = SLEEP;
  }
}

/**
 * Measure water level (average)
 * Process NB_MSRT measures with a 500ms delay between it and save the average value as water level
 */
void measure() {
  water = 0;
  for (int i = 0; i < NB_MSRT; ++i) {
    water += analogRead(PIN_SENSOR_HYG);
    delay(500);
  }
  water /= NB_MSRT;
  Serial.print("measure : ");
  Serial.println(water);
}

