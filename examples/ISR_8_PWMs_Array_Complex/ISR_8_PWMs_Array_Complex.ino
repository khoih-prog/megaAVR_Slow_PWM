/****************************************************************************************************************************
  ISR_8_PWMs_Array_Complex.ino
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

#include <SimpleTimer.h>              // https://github.com/jfturcot/SimpleTimer

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

/////////////////////////////////////////////////

#define PIN_D0      0
#define PIN_D1      1
#define PIN_D2      2
#define PIN_D3      3
#define PIN_D4      4
#define PIN_D5      5
#define PIN_D6      6

// You can assign pins here. Be careful to select good pin to use or crash, e.g pin 6-11
uint32_t PWM_Pin[] =
{
  LED_BUILTIN, PIN_D0, PIN_D1,  PIN_D2,  PIN_D3,  PIN_D4,  PIN_D5,  PIN_D6
};

#define NUMBER_ISR_PWMS         ( sizeof(PWM_Pin) / sizeof(uint32_t) )

typedef void (*irqCallback)  ();

//////////////////////////////////////////////////////

#define USE_COMPLEX_STRUCT      true

//////////////////////////////////////////////////////

#if USE_COMPLEX_STRUCT

typedef struct
{
  uint32_t      PWM_Pin;
  irqCallback   irqCallbackStartFunc;
  irqCallback   irqCallbackStopFunc;

  uint32_t      PWM_Freq;

  uint32_t      PWM_DutyCycle;

  uint32_t      deltaMicrosStart;
  uint32_t      previousMicrosStart;

  uint32_t      deltaMicrosStop;
  uint32_t      previousMicrosStop;

} ISR_PWM_Data;

// In nRF52, avoid doing something fancy in ISR, for example Serial.print()
// The pure simple Serial.prints here are just for demonstration and testing. Must be eliminate in working environment
// Or you can get this run-time error / crash

void doingSomethingStart(int index);

void doingSomethingStop(int index);

#else   // #if USE_COMPLEX_STRUCT

volatile unsigned long deltaMicrosStart    [] = { 0, 0, 0, 0, 0, 0, 0, 0 };
volatile unsigned long previousMicrosStart [] = { 0, 0, 0, 0, 0, 0, 0, 0 };

volatile unsigned long deltaMicrosStop     [] = { 0, 0, 0, 0, 0, 0, 0, 0 };
volatile unsigned long previousMicrosStop  [] = { 0, 0, 0, 0, 0, 0, 0, 0 };


// You can assign any interval for any timer here, in Microseconds
uint32_t PWM_Period[] =
{
  1000L,   500L,   333L,   250L,   200L,   166L,   142L,   125L
};

// You can assign any interval for any timer here, in Hz
float PWM_Freq[] =
{
  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
};

// You can assign any interval for any timer here, in Microseconds
float PWM_DutyCycle[] =
{
  5.0, 10.0, 20.0, 30.0, 40.0, 45.0, 50.0, 55.0
};

void doingSomethingStart(int index)
{
  unsigned long currentMicros  = micros();

  deltaMicrosStart[index]    = currentMicros - previousMicrosStart[index];
  previousMicrosStart[index] = currentMicros;
}

void doingSomethingStop(int index)
{
  unsigned long currentMicros  = micros();

  // Count from start to stop PWM pulse
  deltaMicrosStop[index]    = currentMicros - previousMicrosStart[index];
  previousMicrosStop[index] = currentMicros;
}

#endif    // #if USE_COMPLEX_STRUCT

////////////////////////////////////
// Shared
////////////////////////////////////

void doingSomethingStart0()
{
  doingSomethingStart(0);
}

void doingSomethingStart1()
{
  doingSomethingStart(1);
}

void doingSomethingStart2()
{
  doingSomethingStart(2);
}

void doingSomethingStart3()
{
  doingSomethingStart(3);
}

void doingSomethingStart4()
{
  doingSomethingStart(4);
}

void doingSomethingStart5()
{
  doingSomethingStart(5);
}

void doingSomethingStart6()
{
  doingSomethingStart(6);
}

void doingSomethingStart7()
{
  doingSomethingStart(7);
}


//////////////////////////////////////////////////////

void doingSomethingStop0()
{
  doingSomethingStop(0);
}

void doingSomethingStop1()
{
  doingSomethingStop(1);
}

void doingSomethingStop2()
{
  doingSomethingStop(2);
}

void doingSomethingStop3()
{
  doingSomethingStop(3);
}

void doingSomethingStop4()
{
  doingSomethingStop(4);
}

void doingSomethingStop5()
{
  doingSomethingStop(5);
}

void doingSomethingStop6()
{
  doingSomethingStop(6);
}

void doingSomethingStop7()
{
  doingSomethingStop(7);
}


//////////////////////////////////////////////////////

#if USE_COMPLEX_STRUCT

ISR_PWM_Data curISR_PWM_Data[] =
{
  // pin, irqCallbackStartFunc, irqCallbackStopFunc, PWM_Freq, PWM_DutyCycle, deltaMicrosStart, previousMicrosStart, deltaMicrosStop, previousMicrosStop
  { LED_BUILTIN,  doingSomethingStart0,    doingSomethingStop0,    1,   5, 0, 0, 0, 0 },
  { PIN_D0,       doingSomethingStart1,    doingSomethingStop1,    2,  10, 0, 0, 0, 0 },
  { PIN_D1,       doingSomethingStart2,    doingSomethingStop2,    3,  20, 0, 0, 0, 0 },
  { PIN_D2,       doingSomethingStart3,    doingSomethingStop3,    4,  30, 0, 0, 0, 0 },
  { PIN_D3,       doingSomethingStart4,    doingSomethingStop4,    5,  40, 0, 0, 0, 0 },
  { PIN_D4,       doingSomethingStart5,    doingSomethingStop5,    6,  45, 0, 0, 0, 0 },
  { PIN_D5,       doingSomethingStart6,    doingSomethingStop6,    7,  50, 0, 0, 0, 0 },
  { PIN_D6,       doingSomethingStart7,    doingSomethingStop7,    8,  55, 0, 0, 0, 0 },
};


void doingSomethingStart(int index)
{
  unsigned long currentMicros  = micros();

  curISR_PWM_Data[index].deltaMicrosStart    = currentMicros - curISR_PWM_Data[index].previousMicrosStart;
  curISR_PWM_Data[index].previousMicrosStart = currentMicros;
}

void doingSomethingStop(int index)
{
  unsigned long currentMicros  = micros();

  //curISR_PWM_Data[index].deltaMicrosStop     = currentMicros - curISR_PWM_Data[index].previousMicrosStop;
  // Count from start to stop PWM pulse
  curISR_PWM_Data[index].deltaMicrosStop     = currentMicros - curISR_PWM_Data[index].previousMicrosStart;
  curISR_PWM_Data[index].previousMicrosStop  = currentMicros;
}

#else   // #if USE_COMPLEX_STRUCT

irqCallback irqCallbackStartFunc[] =
{
  doingSomethingStart0,  doingSomethingStart1,  doingSomethingStart2,  doingSomethingStart3,
  doingSomethingStart4,  doingSomethingStart5,  doingSomethingStart6,  doingSomethingStart7
};

irqCallback irqCallbackStopFunc[] =
{
  doingSomethingStop0,  doingSomethingStop1,  doingSomethingStop2,  doingSomethingStop3,
  doingSomethingStop4,  doingSomethingStop5,  doingSomethingStop6,  doingSomethingStop7
};

#endif    // #if USE_COMPLEX_STRUCT

//////////////////////////////////////////////////////

#define SIMPLE_TIMER_MS        2000L

// Init SimpleTimer
SimpleTimer simpleTimer;

// Here is software Timer, you can do somewhat fancy stuffs without many issues.
// But always avoid
// 1. Long delay() it just doing nothing and pain-without-gain wasting CPU power.Plan and design your code / strategy ahead
// 2. Very long "do", "while", "for" loops without predetermined exit time.
void simpleTimerDoingSomething2s()
{
  static unsigned long previousMicrosStart = startMicros;

  unsigned long currMicros = micros();

  Serial.print(F("SimpleTimer (us): "));
  Serial.print(SIMPLE_TIMER_MS);
  Serial.print(F(", us : "));
  Serial.print(currMicros);
  Serial.print(F(", Dus : "));
  Serial.println(currMicros - previousMicrosStart);

  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    Serial.print(F("PWM Channel : "));
    Serial.print(i);
    Serial.print(F(", prog Period (ms): "));

    Serial.print(1000.f / curISR_PWM_Data[i].PWM_Freq);

    Serial.print(F(", actual : "));
    Serial.print((uint32_t) curISR_PWM_Data[i].deltaMicrosStart);

    Serial.print(F(", prog DutyCycle : "));

    Serial.print(curISR_PWM_Data[i].PWM_DutyCycle);

    Serial.print(F(", actual : "));
    Serial.println((float) curISR_PWM_Data[i].deltaMicrosStop * 100.0f / curISR_PWM_Data[i].deltaMicrosStart);
    //Serial.print(F(", actual deltaMicrosStop : ")); Serial.println(curISR_PWM_Data[i].deltaMicrosStop);
    //Serial.print(F(", actual deltaMicrosStart : ")); Serial.println(curISR_PWM_Data[i].deltaMicrosStart);

#else

    Serial.print(F("PWM Channel : "));
    Serial.print(i);

    Serial.print(1000 / PWM_Freq[i]);

    Serial.print(F(", prog. Period (us): "));
    Serial.print(PWM_Period[i]);
    Serial.print(F(", actual : "));
    Serial.print(deltaMicrosStart[i]);

    Serial.print(F(", prog DutyCycle : "));

    Serial.print(PWM_DutyCycle[i]);

    Serial.print(F(", actual : "));
    Serial.println( (float) deltaMicrosStop[i] * 100.0f / deltaMicrosStart[i]);
    //Serial.print(F(", actual deltaMicrosStop : ")); Serial.println(deltaMicrosStop[i]);
    //Serial.print(F(", actual deltaMicrosStart : ")); Serial.println(deltaMicrosStart[i]);
#endif
  }

  previousMicrosStart = currMicros;
}

void setup()
{
  Serial.begin(115200);

  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_8_PWMs_Array_Complex on "));
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

  startMicros = micros();

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_PWM

  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    curISR_PWM_Data[i].previousMicrosStart = startMicros;
    //ISR_PWM.setInterval(curISR_PWM_Data[i].PWM_Period, curISR_PWM_Data[i].irqCallbackStartFunc);

    //void setPWM(uint32_t pin, float frequency, float dutycycle
    // , timer_callback_p StartCallback = nullptr, timer_callback_p StopCallback = nullptr)

    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(curISR_PWM_Data[i].PWM_Pin, curISR_PWM_Data[i].PWM_Freq, curISR_PWM_Data[i].PWM_DutyCycle,
                   curISR_PWM_Data[i].irqCallbackStartFunc, curISR_PWM_Data[i].irqCallbackStopFunc);

#else
    previousMicrosStart[i] = micros();

    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin[i], PWM_Freq[i], PWM_DutyCycle[i], irqCallbackStartFunc[i], irqCallbackStopFunc[i]);

#endif
  }

  // You need this timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary.
  simpleTimer.setInterval(SIMPLE_TIMER_MS, simpleTimerDoingSomething2s);
}

#define BLOCKING_TIME_MS      10000L

void loop()
{
  // This unadvised blocking task is used to demonstrate the blocking effects onto the execution and accuracy to Software timer
  // You see the time elapse of ISR_PWM still accurate, whereas very unaccurate for Software Timer
  // The time elapse for 2000ms software timer now becomes 3000ms (BLOCKING_TIME_MS)
  // While that of ISR_PWM is still prefect.
  delay(BLOCKING_TIME_MS);

  // You need this Software timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary
  // You don't need to and never call ISR_PWM.run() here in the loop(). It's already handled by ISR timer.
  simpleTimer.run();
}
