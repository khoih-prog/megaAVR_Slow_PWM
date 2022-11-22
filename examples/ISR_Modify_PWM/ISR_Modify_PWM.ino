/****************************************************************************************************************************
  ISR_Modify_PWM.ino
  For Arduino megaAVR ATMEGA4809-based boards (UNO WiFi Rev2, NANO_EVERY, etc. )
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/megaAVR_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one megaAVR-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

#if !( defined(__AVR_ATmega4809__) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_AVR_NANO_EVERY) || \
      defined(ARDUINO_AVR_ATmega4809) || defined(ARDUINO_AVR_ATmega4808) || defined(ARDUINO_AVR_ATmega3209) || \
      defined(ARDUINO_AVR_ATmega3208) || defined(ARDUINO_AVR_ATmega1609) || defined(ARDUINO_AVR_ATmega1608) || \
      defined(ARDUINO_AVR_ATmega809) || defined(ARDUINO_AVR_ATmega808) )
#error This is designed only for Arduino or MegaCoreX megaAVR board! Please check your Tools->Board setting
#endif

// These define's must be placed at the beginning before #include "megaAVR_Slow_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_      3

// Select USING_16MHZ     == true for  16MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_8MHZ      == true for   8MHz to Timer TCBx => shorter timer, but better accuracy
// Select USING_250KHZ    == true for 250KHz to Timer TCBx => shorter timer, but better accuracy
// Not select for default 250KHz to Timer TCBx => longer timer,  but worse accuracy
#define USING_16MHZ     true
#define USING_8MHZ      false
#define USING_250KHZ    false

#define USE_TIMER_0     false
#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false

#define USING_MICROS_RESOLUTION       true  //false 

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "megaAVR_Slow_PWM.h"

#define LED_OFF             HIGH
#define LED_ON              LOW

#ifndef LED_BUILTIN
  #define LED_BUILTIN       13
#endif

#ifndef LED_BLUE
  #define LED_BLUE          10
#endif

#ifndef LED_RED
  #define LED_RED           11
#endif

#define USING_HW_TIMER_INTERVAL_MS        false   //true

// Don't change these numbers to make higher Timer freq. System can hang
#define HW_TIMER_INTERVAL_MS        0.0333f
#define HW_TIMER_INTERVAL_FREQ      30000L

volatile uint32_t startMicros = 0;

// Init megaAVR_SLOW_PWM, each can service 16 different ISR-based PWM channels
megaAVR_SLOW_PWM ISR_PWM;

//////////////////////////////////////////////////////

void TimerHandler()
{
  ISR_PWM.run();
}

//////////////////////////////////////////////////////

#define USING_PWM_FREQUENCY     false //true

//////////////////////////////////////////////////////

// You can assign pins here. Be carefull to select good pin to use or crash
uint32_t PWM_Pin    = LED_BUILTIN;

// You can assign any interval for any timer here, in Hz
float PWM_Freq1   = 200.0f;   //1.0f;
// You can assign any interval for any timer here, in Hz
float PWM_Freq2   = 100.0f;   //2.0f;

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period1 = 1000000 / PWM_Freq1;
// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period2 = 1000000 / PWM_Freq2;

// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle1  = 1.0f;     //50.0f;
// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle2  = 5.55f;    //90.0f;

// Channel number used to identify associated channel
int channelNum;

////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_Modify_PWM on "));
  Serial.println(BOARD_NAME);
  Serial.println(MEGA_AVR_SLOW_PWM_VERSION);
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));

  Serial.print(F("TCB Clock Frequency = "));

#if USING_16MHZ
  Serial.println(F("16MHz for highest accuracy"));
#elif USING_8MHZ
  Serial.println(F("8MHz for very high accuracy"));
#else
  Serial.println(F("250KHz for lower accuracy but longer time"));
#endif

#if USING_HW_TIMER_INTERVAL_MS

#if USE_TIMER_1

  ITimer1.init();

  // Using ATmega328 used in UNO => 16MHz CPU clock ,

  if (ITimer1.attachInterruptInterval(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting  ITimer1 OK, micros() = "));
    Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

#elif USE_TIMER_3

  ITimer3.init();

  if (ITimer3.attachInterruptInterval(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting  ITimer3 OK, micros() = "));
    Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer3. Select another freq. or timer"));

#endif

#else

#if USE_TIMER_1

  ITimer1.init();

  // Using ATmega328 used in UNO => 16MHz CPU clock ,

  if (ITimer1.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer1 OK, micros() = "));
    Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

#elif USE_TIMER_2

  ITimer2.init();

  if (ITimer2.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer2 OK, micros() = "));
    Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer2. Select another freq. or timer"));

#elif USE_TIMER_3

  ITimer3.init();

  if (ITimer3.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer3 OK, micros() = "));
    Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer3. Select another freq. or timer"));

#endif    // USE_TIMER_1

#endif    // USING_HW_TIMER_INTERVAL_MS

  Serial.print(F("Using PWM Freq = "));
  Serial.print(PWM_Freq1);
  Serial.print(F(", PWM DutyCycle = "));
  Serial.println(PWM_DutyCycle1);

#if USING_PWM_FREQUENCY

  // You can use this with PWM_Freq in Hz
  ISR_PWM.setPWM(PWM_Pin, PWM_Freq1, PWM_DutyCycle1);

#else
#if USING_MICROS_RESOLUTION
  // Or using period in microsecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1, PWM_DutyCycle1);
#else
  // Or using period in millisecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1 / 1000, PWM_DutyCycle1);
#endif
#endif
}

////////////////////////////////////////////////

void changePWM()
{
  static uint8_t count = 1;

  float PWM_Freq;
  float PWM_DutyCycle;

  if (count++ % 2)
  {
    PWM_Freq        = PWM_Freq2;
    PWM_DutyCycle   = PWM_DutyCycle2;
  }
  else
  {
    PWM_Freq        = PWM_Freq1;
    PWM_DutyCycle   = PWM_DutyCycle1;
  }

  // You can use this with PWM_Freq in Hz
  if (!ISR_PWM.modifyPWMChannel(channelNum, PWM_Pin, PWM_Freq, PWM_DutyCycle))
  {
    Serial.println(F("modifyPWMChannel error for PWM_Period"));
  }
}

////////////////////////////////////////////////

void changingPWM()
{
  static unsigned long changingPWM_timeout = 0;

  static unsigned long current_millis;

#define CHANGING_PWM_INTERVAL    10000L

  current_millis = millis();

  // changePWM every CHANGING_PWM_INTERVAL (10) seconds.
  if ( (current_millis > changingPWM_timeout) )
  {
    if (changingPWM_timeout > 0)
      changePWM();

    changingPWM_timeout = current_millis + CHANGING_PWM_INTERVAL;
  }
}

////////////////////////////////////////////////

void loop()
{
  changingPWM();
}
