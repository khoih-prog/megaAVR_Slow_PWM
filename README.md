# megaAVR_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/megaAVR_Slow_PWM.svg?)](https://www.ardu-badge.com/megaAVR_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/megaAVR_Slow_PWM.svg)](https://github.com/khoih-prog/megaAVR_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/megaAVR_Slow_PWM/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/megaAVR_Slow_PWM.svg)](http://github.com/khoih-prog/megaAVR_Slow_PWM/issues)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>
<a href="https://profile-counter.glitch.me/khoih-prog/count.svg" title="Total khoih-prog Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog/count.svg" style="height: 30px;width: 200px;"></a>
<a href="https://profile-counter.glitch.me/khoih-prog-megaAVR_Slow_PWM/count.svg" title="megaAVR_Slow_PWM Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog-megaAVR_Slow_PWM/count.svg" style="height: 30px;width: 200px;"></a>



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
  * [6. ISR_8_PWMs_Array_Complex on MegaCoreX Nano Every](#6-ISR_8_PWMs_Array_Complex-on-MegaCoreX-Nano-Every)
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


```cpp
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

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long milliseconds) while **the accuracy is nearly perfect** compared to software PWM channels. 

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

1. **megaAVR-based boards** such as **UNO WiFi Rev2, AVR_Nano_Every, etc.**, using [`Arduino megaAVR core`](https://github.com/arduino/ArduinoCore-megaavr)
2. **megaAVR-based boards** such as **UNO WiFi Rev2, AVR_Nano_Every, ATmega4809, ATmega4808, ATmega3209, ATmega3208, ATmega1609, ATmega1608, ATmega809, ATmega808, etc.**, using [`MegaCoreX megaAVR core`](https://github.com/MCUdude/MegaCoreX)

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
2. [`Arduino megaAVR core 1.8.7+`](https://github.com/arduino/ArduinoCore-megaavr/releases) for Arduino megaAVR boards. Use Arduino Board Manager to install. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-megaavr.svg)](https://github.com/arduino/ArduinoCore-megaavr/releases/latest).
3. [`MegaCoreX megaAVR core 1.1.0+`](https://github.com/MCUdude/MegaCoreX/releases) for Arduino megaAVR boards.  [![GitHub release](https://img.shields.io/github/release/MCUdude/MegaCoreX.svg)](https://github.com/MCUdude/MegaCoreX/releases/latest). Follow [**How to install**](https://github.com/MCUdude/MegaCoreX#how-to-install).
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
3. Install [**megaAVR_Slow_PWM** library](https://registry.platformio.org/libraries/khoih-prog/megaAVR_Slow_PWM) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/megaAVR_Slow_PWM/installation). Search for **megaAVR_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```cpp
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "megaAVR_Slow_PWM.hpp"     //https://github.com/khoih-prog/megaAVR_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```cpp
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

```cpp
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

```cpp
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

https://github.com/khoih-prog/megaAVR_Slow_PWM/blob/d719f9f1776e237b12c24c5853c88cd6f165d36f/examples/ISR_8_PWMs_Array_Complex/ISR_8_PWMs_Array_Complex.ino#L16-L516


---
---

### Debug Terminal Output Samples

### 1. ISR_8_PWMs_Array_Complex on megaAVR Nano Every

The following is the sample terminal output when running example [ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex) **megaAVR Nano Every** to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```cpp
Starting ISR_8_PWMs_Array_Complex on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.1
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

```cpp
Starting ISR_8_PWMs_Array on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.1
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

```cpp
Starting ISR_8_PWMs_Array_Simple on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.1
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

```cpp
Starting ISR_Modify_PWM on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.1
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
Using PWM Freq = 200.00, PWM DutyCycle = 1.00
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 2018636
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 12026148
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 22018624
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 32026252
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 42023732
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 52031224
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 62023700
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 72031192
```

---

### 5. ISR_Changing_PWM on megaAVR Nano Every

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/ISR_Changing_PWM) on **megaAVR Nano Every** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```cpp
Starting ISR_Changing_PWM on megaAVR Nano Every
megaAVR_SLOW_PWM v1.2.1
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
Starting  ITimer1 OK, micros() = 2012575
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2018587
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 12026819
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 22033842
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 32041005
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 42048026
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 52055177
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 62062218
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 72069373
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 82076404
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 92083560
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 102090588
```

---

### 6. ISR_8_PWMs_Array_Complex on MegaCoreX Nano Every

The following is the sample terminal output when running example [ISR_8_PWMs_Array_Complex](examples/ISR_8_PWMs_Array_Complex) **MegaCoreX Nano Every** to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```cpp
Starting ISR_8_PWMs_Array_Complex on MegaCoreX Nano Every
megaAVR_SLOW_PWM v1.2.1
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
6. Add support to [`MegaCoreX megaAVR core`](https://github.com/MCUdude/MegaCoreX) core, including ATmega4809, ATmega4808, ATmega3209, ATmega3208, ATmega1609, ATmega1608, ATmega809 and ATmega808
7. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
8. DutyCycle to be optionally updated at the end current PWM period instead of immediately.
9. Display informational warning only when `_PWM_LOGLEVEL_` > 3

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

Copyright (c) 2021- Khoi Hoang


