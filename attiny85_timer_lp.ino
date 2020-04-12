/* ATtiny85 Low-Power Timer */

#include <avr/sleep.h>
#include <avr/interrupt.h>

//#define DEBUG_VERBOSE
    
// Utility macro
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

// constants 
const int pin_buzzer = 0;   // buzzer pin
const int pin_led    = 1;   // led pin
const int pin_switch = 2;   // INT0
const int pin_debug  = 3;

const unsigned long duration = 1200000; // alarm time - 20 minutes
//const unsigned long duration = 10000; // alarm time - 10 sec
unsigned long startTime = 0;

bool isOpen = false;


void sleepNow()
{
  detachInterrupt(0);
  attachInterrupt(0, wakeUpNow, LOW);
  sleep_enable();                          // enables the sleep bit in the mcucr register so sleep is possible
  sleep_cpu();                          // here the device is actually put to sleep!!
  sleep_disable();                       // first thing after waking from sleep: disable sleep...
  detachInterrupt(0);
  attachInterrupt(0, closing, RISING);
}

void wakeUpNow()  // here the interrupt is handled after wakeup
{
  startTime = millis(); // resets time counting
  isOpen = true;
#ifdef DEBUG_VERBOSE
 digitalWrite(pin_debug, HIGH);
#endif
}

void closing()  // here the interrupt is handled after closing
{
  isOpen = false;
#ifdef DEBUG_VERBOSE
 digitalWrite(pin_debug, LOW);
#endif
}


void setup () {
  pinMode(pin_led,    OUTPUT);
  pinMode(pin_buzzer, OUTPUT);
  pinMode(pin_switch, INPUT_PULLUP);
//  attachInterrupt(0, wakeUpNow, LOW);     // use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW
#ifdef DEBUG_VERBOSE
  pinMode(pin_debug,  OUTPUT);
#else
  pinMode(pin_debug,  INPUT_PULLUP);
#endif
  pinMode(4, INPUT_PULLUP);
  adc_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
}


void playBeep (void)
{
  for(int i = 0; i < 3; i++)
  {
    for (int j=0; j < 1500; j++)
    {
      digitalWrite(pin_buzzer, HIGH);
      delay(1);
      digitalWrite(pin_buzzer, LOW);
      delay(1);
    }
    delay(3000);
  }
}

void flashLed (int wait) {
  digitalWrite(pin_led, HIGH);
  delay(wait);
  digitalWrite(pin_led, LOW);
}

// Main loop ----------------------------------------------

void loop ()
{
  do {
    // Flash light
    flashLed(20);
    delay(3000);
  } while (millis() - startTime < duration && isOpen);
  // Alarm
  if(isOpen)
    playBeep();
  sleepNow();
}
