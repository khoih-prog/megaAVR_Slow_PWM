# megaAVR_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/megaAVR_Slow_PWM.svg?)](https://www.ardu-badge.com/megaAVR_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/megaAVR_Slow_PWM.svg)](https://github.com/khoih-prog/megaAVR_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/megaAVR_Slow_PWM/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/megaAVR_Slow_PWM.svg)](http://github.com/khoih-prog/megaAVR_Slow_PWM/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>


---
---

## Table of Contents

* [Important Change from v1.2.0](#Important-Change-from-v120)
* [Why do we need this megaAVR_Slow_PWM library](#why-do-we-need-this-megaAVR_Slow_PWM-library)
  * [Features](#features)
  * [Why using ISR-based PWM is better](#why-using-isr-based-pwm-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [More useful Information](#more-useful-information)
  * [1. Documents](#1-documents)
  * [2. Timer TCB0-TCB3](#2-timer-tcb0-tcb3)
* [Usage](#usage)
  * [1. Init Hardware Timer](#1-init-hardware-timer)
  * [2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions](#2-Set-PWM-Frequency-dutycycle-attach-irqCallbackStartFunc-and-irqCallbackStopFunc-functions)
* [Examples](#examples)
  * [ 1. ISR_8_PWMs_Array](examples/ISR_8_PWMs_Array)
  * [ 2. ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex)
  * [ 3. ISR_8_PWMs_Array_Simple](examples/ISR_8_PWMs_Array_Simple)
  * [ 4. ISR_Changing_PWM](examples/ISR_Changing_PWM)
  * [ 5. ISR_Modify_PWM](examples/ISR_Modify_PWM)
  * [ 6. multiFileProject](examples/multiFileProject). **New**
* [Example ISR_8_PWMs_Array_Complex](#Example-ISR_8_PWMs_Array_Complex)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_8_PWMs_Array_Complex on megaAVR Nano Every](#1-ISR_8_PWMs_Array_Complex-on-megaAVR-Nano-Every)
  * [2. ISR_8_PWMs_Array on megaAVR Nano Every](#2-isr_8_pwms_array-on-megaAVR-Nano-Every)
  * [3. ISR_8_PWMs_Array_Simple on megaAVR Nano Every](#3-ISR_8_PWMs_Array_Simple-on-megaAVR-Nano-Every)
  * [4. ISR_Modify_PWM on megaAVR Nano Every](#4-ISR_Modify_PWM-on-megaAVR-Nano-Every)
  * [5. ISR_Changing_PWM on megaAVR Nano Every](#5-ISR_Changing_PWM-on-megaAVR-Nano-Every)
  * [6. ISR_Changing_PWM on MegaCoreX Nano Every](#6-ISR_Changing_PWM-on-MegaCoreX-Nano-Every)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.2.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

As more complex calculation and check **inside ISR** are introduced from v1.2.0, there is possibly some crash depending on use-case.

You can modify to use larger `HW_TIMER_INTERVAL_US`, (from current 33.3uS), according to your board and use-case if crash happens.


```
// Don't change these numbers to make higher Timer freq. System can hang
#define HW_TIMER_INTERVAL_MS        0.0333f
#define HW_TIMER_INTERVAL_FREQ      30000L
```

---
---

### Why do we need this [megaAVR_Slow_PWM library](https://github.com/khoih-prog/megaAVR_Slow_PWM)

### Features

This library enables you to use ISR-based PWM channels on ATmega4809-based boards, such as UNO WiFi Rev2, AVR_Nano_Every, etc., using [`Arduino megaAVR core`](https://github.com/arduino/ArduinoCore-megaavr) to create and output PWM any GPIO pin. Because this library doesn't use the powerful purely hardware-controlled PWM with many limitations, the maximum PWM frequency is currently limited at **500Hz**, which is still suitable for many real-life applications. Now you can change the PWM settings on-the-fly

---

This library enables you to use Interrupt from Hardware Timers on megaAVR-based boards to create and output PWM to pins. It now supports 16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer. PWM interval can be very long (uint64_t microsecs / millisecs). The most important feature is they're ISR-based PWM channels. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software PWM using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software PWM channels. 

The most important feature is they're ISR-based PWM channels. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**ISR_8_PWMs_Array_Complex**](examples/ISR_8_PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy, compared to software PWM, by printing the actual period / duty-cycle in `microsecs` of each of PWM-channels.

Being ISR-based PWM, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet or Blynk services. You can also have many `(up to 16)` PWM channels to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see `software-based` SimpleTimer is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

#### Why using ISR-based PWM is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software PWM channels using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software PWM channels, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

1. **megaAVR-based boards** such as **UNO WiFi Rev2, AVR_Nano_Every, etc.**, etc., using [`Arduino megaAVR core`](https://github.com/arduino/ArduinoCore-megaavr)

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
2. [`Arduino megaAVR core 1.8.7+`](https://github.com/arduino/ArduinoCore-megaavr/releases) for Arduino megaAVR boards. Use Arduino Board Manager to install.
3. [`MegaCoreX megaAVR core 1.0.9+`](https://github.com/MCUdude/MegaCoreX/releases) for Arduino megaAVR boards.  [![GitHub release](https://img.shields.io/github/release/MCUdude/MegaCoreX.svg)](https://github.com/MCUdude/MegaCoreX/releases/latest). Follow [**How to install**](https://github.com/MCUdude/MegaCoreX#how-to-install).
4. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) for [ISR_Timers_Array_Simple](examples/ISR_Timers_Array_Simple) and [ISR_16_Timers_Array_Complex](examples/ISR_16_Timers_Array_Complex) examples.
   
   
---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**megaAVR_Slow_PWM**](https://github.com/khoih-prog/megaAVR_Slow_PWM), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/megaAVR_Slow_PWM.svg?)](https://www.ardu-badge.com/megaAVR_Slow_PWM) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**megaAVR_Slow_PWM**](https://github.com/khoih-prog/megaAVR_Slow_PWM) page.
2. Download the latest release `megaAVR_Slow_PWM-master.zip`.
3. Extract the zip file to `megaAVR_Slow_PWM-master` directory 
4. Copy whole `megaAVR_Slow_PWM-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**megaAVR_Slow_PWM** library](https://platformio.org/lib/show/12886/megaAVR_Slow_PWM) by using [Library Manager](https://platformio.org/lib/show/12886/megaAVR_Slow_PWM/installation). Search for **megaAVR_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "megaAVR_Slow_PWM.hpp"     //https://github.com/khoih-prog/megaAVR_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "megaAVR_Slow_PWM.h"           //https://github.com/khoih-prog/megaAVR_Slow_PWM
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)

---
---

## More useful Information

### 1. Documents

1. [Arduino 101: Timers and Interrupts](https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072)
2. [megaAVR0-series-Family-Data-Sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf)

### 2. Timer TCB0-TCB3

TCB0-TCB3 are 16-bit timers.

---
---

## Usage


Before using any Timer, you have to make sure the Timer has not been used by any other purpose.


#### 1. Init Hardware Timer

```
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

// Init megaAVR_SLOW_PWM, each can service 16 different ISR-based PWM channels
megaAVR_SLOW_PWM ISR_PWM;
```

#### 2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions

```
void irqCallbackStartFunc()
{

}

void irqCallbackStopFunc()
{

}

void setup()
{
  ....
  
  // You can use this with PWM_Freq in Hz
  ISR_PWM.setPWM(PWM_Pin, PWM_Freq, PWM_DutyCycle, irqCallbackStartFunc, irqCallbackStopFunc);
                   
  ....                 
}  
```

---
---

### Examples: 

 1. [ISR_8_PWMs_Array](examples/ISR_8_PWMs_Array)
 2. [ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex)
 3. [ISR_8_PWMs_Array_Simple](examples/ISR_8_PWMs_Array_Simple)
 4. [ISR_Changing_PWM](examples/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/ISR_Modify_PWM)
 6. [**multiFileProject**](examples/multiFileProject) **New**

 
---
---

### Example [ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex)

```
#if !( defined(__AVR_ATmega4809__) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_AVR_NANO_EVERY) || \
      defined(ARDUINO_AVR_ATmega4809) || defined(ARDUINO_AVR_ATmega4808) || defined(ARDUINO_AVR_ATmega3209) || \
      defined(ARDUINO_AVR_ATmega3208) || defined(ARDUINO_AVR_ATmega1609) || defined(ARDUINO_AVR_ATmega1608) || \
      defined(ARDUINO_AVR_ATmega809) || defined(ARDUINO_AVR_ATmega808) )
  #error This is designed only for Arduino or MegaCoreX megaAVR board! Please check your Tools->Board setting
#endif

// These define's must be placed at the beginning before #include "ESP32_PWM.h"
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

  Serial.print(F("SimpleTimer (us): ")); Serial.print(SIMPLE_TIMER_MS);
  Serial.print(F(", us : ")); Serial.print(currMicros);
  Serial.print(F(", Dus : ")); Serial.println(currMicros - previousMicrosStart);

  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    Serial.print(F("PWM Channel : ")); Serial.print(i);
    Serial.print(F(", prog Period (ms): "));

    Serial.print(1000.f / curISR_PWM_Data[i].PWM_Freq);

    Serial.print(F(", actual : ")); Serial.print((uint32_t) curISR_PWM_Data[i].deltaMicrosStart);

    Serial.print(F(", prog DutyCycle : "));

    Serial.print(curISR_PWM_Data[i].PWM_DutyCycle);

    Serial.print(F(", actual : ")); Serial.println((float) curISR_PWM_Data[i].deltaMicrosStop * 100.0f / curISR_PWM_Data[i].deltaMicrosStart);
    //Serial.print(F(", actual deltaMicrosStop : ")); Serial.println(curISR_PWM_Data[i].deltaMicrosStop);
    //Serial.print(F(", actual deltaMicrosStart : ")); Serial.println(curISR_PWM_Data[i].deltaMicrosStart);

#else

    Serial.print(F("PWM Channel : ")); Serial.print(i);

    Serial.print(1000 / PWM_Freq[i]);

    Serial.print(F(", prog. Period (us): ")); Serial.print(PWM_Period[i]);
    Serial.print(F(", actual : ")); Serial.print(deltaMicrosStart[i]);

    Serial.print(F(", prog DutyCycle : "));

    Serial.print(PWM_DutyCycle[i]);

    Serial.print(F(", actual : ")); Serial.println( (float) deltaMicrosStop[i] * 100.0f / deltaMicrosStart[i]);
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

  Serial.print(F("\nStarting ISR_8_PWMs_Array_Complex on ")); Serial.println(BOARD_NAME);
  Serial.println(MEGA_AVR_SLOW_PWM_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));

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
    Serial.print(F("Starting  ITimer1 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
    
#elif USE_TIMER_3

  ITimer3.init();

  if (ITimer3.attachInterruptInterval(HW_TIMER_INTERVAL_MS, TimerHandler))
  {
    Serial.print(F("Starting  ITimer3 OK, micros() = ")); Serial.println(micros());
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
    Serial.print(F("Starting  ITimer1 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));

#elif USE_TIMER_2

  ITimer2.init();

  if (ITimer2.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer2 OK, micros() = ")); Serial.println(micros());
  }
  else
    Serial.println(F("Can't set ITimer2. Select another freq. or timer"));
    
#elif USE_TIMER_3

  ITimer3.init();

  if (ITimer3.attachInterrupt(HW_TIMER_INTERVAL_FREQ, TimerHandler))
  {
    Serial.print(F("Starting  ITimer3 OK, micros() = ")); Serial.println(micros());
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
```
---
---

### Debug Terminal Output Samples

### 1. ISR_8_PWMs_Array_Complex on megaAVR Nano Every

The following is the sample terminal output when running example [ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex) **megaAVR Nano Every** to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_8_PWMs_Array_Complex on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.0
CPU Frequency = 16 MHz
TCB Clock Frequency = 16MHz for highest accuracy
[PWM] TCB 1
[PWM] ==================
[PWM] Init, Timer =  1
[PWM] CTRLB   =  0
[PWM] CCMP    =  65535
[PWM] INTCTRL =  0
[PWM] CTRLA   =  1
[PWM] ==================
[PWM] Frequency =  30000.00 , CLK_TCB_FREQ =  16000000
[PWM] setFrequency: _CCMPValueRemaining =  533
Starting  ITimer1 OK, micros() = 2013580
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2015636
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2021248
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2027536
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2033868
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2040224
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2047760
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2056708
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2065380
SimpleTimer (us): 2000, us : 12076620, Dus : 10061960
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000016, prog DutyCycle : 5, actual : 5.00
PWM Channel : 1, prog Period (ms): 500.00, actual : 500020, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333376, prog DutyCycle : 20, actual : 19.97
PWM Channel : 3, prog Period (ms): 250.00, actual : 250008, prog DutyCycle : 30, actual : 30.00
PWM Channel : 4, prog Period (ms): 200.00, actual : 200044, prog DutyCycle : 40, actual : 40.00
PWM Channel : 5, prog Period (ms): 166.67, actual : 166640, prog DutyCycle : 45, actual : 45.01
PWM Channel : 6, prog Period (ms): 142.86, actual : 142872, prog DutyCycle : 50, actual : 50.00
PWM Channel : 7, prog Period (ms): 125.00, actual : 124980, prog DutyCycle : 55, actual : 55.02
SimpleTimer (us): 2000, us : 22180588, Dus : 10103968
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000016, prog DutyCycle : 5, actual : 4.99
PWM Channel : 1, prog Period (ms): 500.00, actual : 500056, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333380, prog DutyCycle : 20, actual : 19.98
PWM Channel : 3, prog Period (ms): 250.00, actual : 249972, prog DutyCycle : 30, actual : 29.99
PWM Channel : 4, prog Period (ms): 200.00, actual : 199992, prog DutyCycle : 40, actual : 39.97
PWM Channel : 5, prog Period (ms): 166.67, actual : 166716, prog DutyCycle : 45, actual : 44.97
PWM Channel : 6, prog Period (ms): 142.86, actual : 142992, prog DutyCycle : 50, actual : 49.90
PWM Channel : 7, prog Period (ms): 125.00, actual : 125108, prog DutyCycle : 55, actual : 54.90
SimpleTimer (us): 2000, us : 32320408, Dus : 10139820
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000084, prog DutyCycle : 5, actual : 4.99
PWM Channel : 1, prog Period (ms): 500.00, actual : 500088, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333388, prog DutyCycle : 20, actual : 19.97
PWM Channel : 3, prog Period (ms): 250.00, actual : 250052, prog DutyCycle : 30, actual : 29.96
PWM Channel : 4, prog Period (ms): 200.00, actual : 200060, prog DutyCycle : 40, actual : 39.97
PWM Channel : 5, prog Period (ms): 166.67, actual : 166640, prog DutyCycle : 45, actual : 44.96
PWM Channel : 6, prog Period (ms): 142.86, actual : 142912, prog DutyCycle : 50, actual : 49.93
PWM Channel : 7, prog Period (ms): 125.00, actual : 125084, prog DutyCycle : 55, actual : 54.91
SimpleTimer (us): 2000, us : 42478424, Dus : 10158016
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000048, prog DutyCycle : 5, actual : 5.00
PWM Channel : 1, prog Period (ms): 500.00, actual : 500020, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333384, prog DutyCycle : 20, actual : 19.97
PWM Channel : 3, prog Period (ms): 250.00, actual : 250076, prog DutyCycle : 30, actual : 29.96
PWM Channel : 4, prog Period (ms): 200.00, actual : 200116, prog DutyCycle : 40, actual : 39.93
PWM Channel : 5, prog Period (ms): 166.67, actual : 166704, prog DutyCycle : 45, actual : 44.97
PWM Channel : 6, prog Period (ms): 142.86, actual : 142944, prog DutyCycle : 50, actual : 49.94
PWM Channel : 7, prog Period (ms): 125.00, actual : 125056, prog DutyCycle : 55, actual : 54.93
SimpleTimer (us): 2000, us : 52642384, Dus : 10163960
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000080, prog DutyCycle : 5, actual : 4.99
PWM Channel : 1, prog Period (ms): 500.00, actual : 500028, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333420, prog DutyCycle : 20, actual : 19.97
PWM Channel : 3, prog Period (ms): 250.00, actual : 250012, prog DutyCycle : 30, actual : 29.97
PWM Channel : 4, prog Period (ms): 200.00, actual : 200016, prog DutyCycle : 40, actual : 39.96
PWM Channel : 5, prog Period (ms): 166.67, actual : 166744, prog DutyCycle : 45, actual : 44.98
PWM Channel : 6, prog Period (ms): 142.86, actual : 142884, prog DutyCycle : 50, actual : 49.94
PWM Channel : 7, prog Period (ms): 125.00, actual : 124988, prog DutyCycle : 55, actual : 54.98
```

---

### 2. ISR_8_PWMs_Array on megaAVR Nano Every

The following is the sample terminal output when running example [**ISR_8_PWMs_Array**](examples/ISR_8_PWMs_Array) on **megaAVR Nano Every** to demonstrate how to use multiple PWM channels with simple callback functions.

```
Starting ISR_8_PWMs_Array on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.0
CPU Frequency = 16 MHz
TCB Clock Frequency = 16MHz for highest accuracy
[PWM] TCB 1
[PWM] ==================
[PWM] Init, Timer =  1
[PWM] CTRLB   =  0
[PWM] CCMP    =  65535
[PWM] INTCTRL =  0
[PWM] CTRLA   =  1
[PWM] ==================
[PWM] Frequency =  30000.00 , CLK_TCB_FREQ =  16000000
[PWM] setFrequency: _CCMPValueRemaining =  533
Starting  ITimer1 OK, micros() = 2012864
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2014864
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2020544
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2026820
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2033152
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2039500
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2046984
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2055924
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2064600
```

---

### 3. ISR_8_PWMs_Array_Simple on megaAVR Nano Every

The following is the sample terminal output when running example [**ISR_8_PWMs_Array_Simple**](examples/ISR_8_PWMs_Array_Simple) on **megaAVR Nano Every** to demonstrate how to use multiple PWM channels.

```
Starting ISR_8_PWMs_Array_Simple on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.0
CPU Frequency = 16 MHz
TCB Clock Frequency = 16MHz for highest accuracy
[PWM] TCB 1
[PWM] ==================
[PWM] Init, Timer =  1
[PWM] CTRLB   =  0
[PWM] CCMP    =  65535
[PWM] INTCTRL =  0
[PWM] CTRLA   =  1
[PWM] ==================
[PWM] Frequency =  30000.00 , CLK_TCB_FREQ =  16000000
[PWM] setFrequency: _CCMPValueRemaining =  533
Starting  ITimer1 OK, micros() = 2012448
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2014448
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2020112
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2026388
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2032672
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2039076
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2046548
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2055500
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2064172
```

---

### 4. ISR_Modify_PWM on megaAVR Nano Every

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/ISR_Modify_PWM) on **megaAVR Nano Every** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.0
CPU Frequency = 16 MHz
TCB Clock Frequency = 16MHz for highest accuracy
[PWM] TCB 1
[PWM] ==================
[PWM] Init, Timer =  1
[PWM] CTRLB   =  0
[PWM] CCMP    =  65535
[PWM] INTCTRL =  0
[PWM] CTRLA   =  1
[PWM] ==================
[PWM] Frequency =  30000.00 , CLK_TCB_FREQ =  16000000
[PWM] setFrequency: _CCMPValueRemaining =  533
Starting  ITimer1 OK, micros() = 2012696
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2018640
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 12019140
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 22019624
```

---

### 5. ISR_Changing_PWM on megaAVR Nano Every

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/ISR_Changing_PWM) on **megaAVR Nano Every** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.0
CPU Frequency = 16 MHz
TCB Clock Frequency = 16MHz for highest accuracy
[PWM] TCB 1
[PWM] ==================
[PWM] Init, Timer =  1
[PWM] CTRLB   =  0
[PWM] CCMP    =  65535
[PWM] INTCTRL =  0
[PWM] CTRLA   =  1
[PWM] ==================
[PWM] Frequency =  30000.00 , CLK_TCB_FREQ =  16000000
[PWM] setFrequency: _CCMPValueRemaining =  533
Starting  ITimer1 OK, micros() = 2012872
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2018824
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 12026916
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 22033944
```

---

### 6. ISR_8_PWMs_Array_Complex on MegaCoreX Nano Every

The following is the sample terminal output when running example [ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex) **MegaCoreX Nano Every** to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_8_PWMs_Array_Complex on MegaCoreX Nano Every
megaAVR_SLOW_PWM v1.2.0
CPU Frequency = 16 MHz
TCB Clock Frequency = 16MHz for highest accuracy
[PWM] TCB 1
[PWM] ==================
[PWM] Init, Timer =  1
[PWM] CTRLB   =  0
[PWM] CCMP    =  65535
[PWM] INTCTRL =  0
[PWM] CTRLA   =  1
[PWM] ==================
[PWM] Frequency =  30000.00 , CLK_TCB_FREQ =  16000000
[PWM] setFrequency: _CCMPValueRemaining =  533
Starting  ITimer1 OK, micros() = 2013258
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2015189
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2020809
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2027083
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2033362
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2039701
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2046513
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2054578
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2063400
SimpleTimer (us): 2000, us : 12073263, Dus : 10058994
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000082, prog DutyCycle : 5, actual : 4.99
PWM Channel : 1, prog Period (ms): 500.00, actual : 500023, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333377, prog DutyCycle : 20, actual : 19.98
PWM Channel : 3, prog Period (ms): 250.00, actual : 250008, prog DutyCycle : 30, actual : 29.99
PWM Channel : 4, prog Period (ms): 200.00, actual : 200010, prog DutyCycle : 40, actual : 39.99
PWM Channel : 5, prog Period (ms): 166.67, actual : 166667, prog DutyCycle : 45, actual : 45.00
PWM Channel : 6, prog Period (ms): 142.86, actual : 142869, prog DutyCycle : 50, actual : 50.00
PWM Channel : 7, prog Period (ms): 125.00, actual : 125015, prog DutyCycle : 55, actual : 55.01
SimpleTimer (us): 2000, us : 22182603, Dus : 10109340
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000049, prog DutyCycle : 5, actual : 4.99
PWM Channel : 1, prog Period (ms): 500.00, actual : 500022, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333410, prog DutyCycle : 20, actual : 19.98
PWM Channel : 3, prog Period (ms): 250.00, actual : 250049, prog DutyCycle : 30, actual : 29.98
PWM Channel : 4, prog Period (ms): 200.00, actual : 200058, prog DutyCycle : 40, actual : 39.97
PWM Channel : 5, prog Period (ms): 166.67, actual : 166641, prog DutyCycle : 45, actual : 44.96
PWM Channel : 6, prog Period (ms): 142.86, actual : 142910, prog DutyCycle : 50, actual : 49.93
PWM Channel : 7, prog Period (ms): 125.00, actual : 125054, prog DutyCycle : 55, actual : 54.93
SimpleTimer (us): 2000, us : 32535226, Dus : 10352623
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000049, prog DutyCycle : 5, actual : 4.99
PWM Channel : 1, prog Period (ms): 500.00, actual : 500026, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333385, prog DutyCycle : 20, actual : 19.98
PWM Channel : 3, prog Period (ms): 250.00, actual : 250013, prog DutyCycle : 30, actual : 29.98
PWM Channel : 4, prog Period (ms): 200.00, actual : 200016, prog DutyCycle : 40, actual : 39.96
PWM Channel : 5, prog Period (ms): 166.67, actual : 166673, prog DutyCycle : 45, actual : 44.98
PWM Channel : 6, prog Period (ms): 142.86, actual : 142877, prog DutyCycle : 50, actual : 49.94
PWM Channel : 7, prog Period (ms): 125.00, actual : 125057, prog DutyCycle : 55, actual : 54.94
SimpleTimer (us): 2000, us : 42973223, Dus : 10437997
PWM Channel : 0, prog Period (ms): 1000.00, actual : 1000015, prog DutyCycle : 5, actual : 5.00
PWM Channel : 1, prog Period (ms): 500.00, actual : 500060, prog DutyCycle : 10, actual : 9.99
PWM Channel : 2, prog Period (ms): 333.33, actual : 333346, prog DutyCycle : 20, actual : 19.97
PWM Channel : 3, prog Period (ms): 250.00, actual : 249974, prog DutyCycle : 30, actual : 29.97
PWM Channel : 4, prog Period (ms): 200.00, actual : 200042, prog DutyCycle : 40, actual : 39.98
PWM Channel : 5, prog Period (ms): 166.67, actual : 166633, prog DutyCycle : 45, actual : 44.99
PWM Channel : 6, prog Period (ms): 142.86, actual : 142872, prog DutyCycle : 50, actual : 49.98
PWM Channel : 7, prog Period (ms): 125.00, actual : 125017, prog DutyCycle : 55, actual : 55.01
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level `_PWM_LOGLEVEL_` from 0 to 4

```cpp
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [megaAVR_Slow_PWM issues](https://github.com/khoih-prog/megaAVR_Slow_PWM/issues)

---

## TO DO

1. Search for bug and improvement.
2. Similar features for remaining Arduino boards

---

## DONE

1. Basic hardware multi-channel PWM for **Arduino megaAVR boards, such as UNO WiFi Rev2, AVR_Nano_Every, etc.** using [`Arduino megaAVR core`](https://github.com/arduino/ArduinoCore-megaavr)
2. Add Table of Contents
3. Add functions to modify PWM settings on-the-fly
4. Fix `multiple-definitions` linker error
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. Add support to `MegaCoreX` core, including ATmega4809, ATmega4808, ATmega3209, ATmega3208, ATmega1609, ATmega1608, ATmega809 and ATmega808
7. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
8. DutyCycle to be optionally updated at the end current PWM period instead of immediately.


---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/megaAVR_Slow_PWM/blob/main/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


