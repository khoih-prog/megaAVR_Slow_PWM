/****************************************************************************************************************************
  multiFileProject.ino

  For Arduino megaAVR ATMEGA4809-based boards (UNO WiFi Rev2, NANO_EVERY, etc. )
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/megaAVR_Slow_PWM
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#if !( defined(__AVR_ATmega4809__) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_AVR_NANO_EVERY) || \
      defined(ARDUINO_AVR_ATmega4809) || defined(ARDUINO_AVR_ATmega4808) || defined(ARDUINO_AVR_ATmega3209) || \
      defined(ARDUINO_AVR_ATmega3208) || defined(ARDUINO_AVR_ATmega1609) || defined(ARDUINO_AVR_ATmega1608) || \
      defined(ARDUINO_AVR_ATmega809) || defined(ARDUINO_AVR_ATmega808) )
#error This is designed only for Arduino or MegaCoreX megaAVR board! Please check your Tools->Board setting
#endif

#define MEGA_AVR_SLOW_PWM_VERSION_MIN_TARGET      F("megaAVR_SLOW_PWM v1.2.1")
#define MEGA_AVR_SLOW_PWM_VERSION_MIN             1002001

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "megaAVR_Slow_PWM.h"

void setup()
{
  Serial.begin(115200);

  while (!Serial);

  Serial.println("\nStart multiFileProject");
  Serial.println(MEGA_AVR_SLOW_PWM_VERSION);

#if defined(MEGA_AVR_SLOW_PWM_VERSION_MIN)

  if (MEGA_AVR_SLOW_PWM_VERSION_INT < MEGA_AVR_SLOW_PWM_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(MEGA_AVR_SLOW_PWM_VERSION_MIN_TARGET);
  }

#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
}
