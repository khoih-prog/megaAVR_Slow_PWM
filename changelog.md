# megaAVR_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/megaAVR_Slow_PWM.svg?)](https://www.ardu-badge.com/megaAVR_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/megaAVR_Slow_PWM.svg)](https://github.com/khoih-prog/megaAVR_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/megaAVR_Slow_PWM/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/megaAVR_Slow_PWM.svg)](http://github.com/khoih-prog/megaAVR_Slow_PWM/issues)


<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>
<a href="https://profile-counter.glitch.me/khoih-prog/count.svg" title="Total khoih-prog Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog/count.svg" style="height: 30px;width: 200px;"></a>
<a href="https://profile-counter.glitch.me/khoih-prog-megaAVR_Slow_PWM/count.svg" title="megaAVR_Slow_PWM Visitor count"><img src="https://profile-counter.glitch.me/khoih-prog-megaAVR_Slow_PWM/count.svg" style="height: 30px;width: 200px;"></a>


---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.2.1](#releases-v121)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.0](#releases-v110)
  * [Initial Releases v1.0.0](#Initial-Releases-v100)

---
---

## Changelog

### Releases v1.2.1

1. Fix `DutyCycle` bug. Check [float precisison of DutyCycle only sometimes working #3](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/3)
2. Fix `New Period` display bug. Check [random dropouts #4](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/4)
3. Update examples

### Releases v1.2.0

1. Fix `multiple-definitions` linker error. Drop `src_cpp` and `src_h` directories
2. Add support to `MegaCoreX` core, including ATmega4809, ATmega4808, ATmega3209, ATmega3208, ATmega1609, ATmega1608, ATmega809 and ATmega808
3. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`. Check [Change Duty Cycle #1](https://github.com/khoih-prog/ESP8266_PWM/issues/1#issuecomment-1024969658)
4. DutyCycle to be optionally updated at the end current PWM period instead of immediately. Check [DutyCycle to be updated at the end current PWM period #2](https://github.com/khoih-prog/ESP8266_PWM/issues/2)
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. Add example [multiFileProject](examples/multiFileProject) to demo for multiple-file project
7. Update examples accordingly

### Releases v1.1.0

1. Add functions to modify PWM settings on-the-fly
2. Add example to demo how to modify PWM settings on-the-fly

### Initial Releases v1.0.0

1. Initial coding to support **Arduino megaAVR boards, such as UNO WiFi Rev2, AVR_Nano_Every, etc.**, etc. using [`Arduino megaAVR core`](https://github.com/arduino/ArduinoCore-megaavr)

2. The hybrid ISR-based PWM channels can generate from very low (much less than 1Hz) to highest PWM frequencies up to 500Hz with acceptable accuracy.

---
---

## Copyright

Copyright 2021- Khoi Hoang


