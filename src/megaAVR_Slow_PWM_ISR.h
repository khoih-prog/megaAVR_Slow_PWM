/****************************************************************************************************************************
  megaAVR_Slow_PWM_ISR.h
  For Arduino megaAVR ATMEGA4809-based boards (UNO WiFi Rev2, NANO_EVERY, etc. )
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/megaAVR_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one megaAVR-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      27/09/2021 Initial coding for megaAVR-based boards (UNO WiFi Rev2, NANO_EVERY, etc.)
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
  1.2.0   K Hoang      02/02/2022 Fix multiple-definitions linker error. Improve accuracy. Optimize code. Add MegaCoreX
  1.2.1   K Hoang      04/03/2022 Fix `DutyCycle` and `New Period` display bugs. Display warning only when debug level > 3
*****************************************************************************************************************************/

#pragma once

#ifndef MEGA_AVR_SLOW_PWM_ISR_H
#define MEGA_AVR_SLOW_PWM_ISR_H

#include "megaAVR_Slow_PWM_ISR.hpp"
#include "megaAVR_Slow_PWM_ISR_Impl.h"

#endif    // MEGA_AVR_SLOW_PWM_ISR_H


