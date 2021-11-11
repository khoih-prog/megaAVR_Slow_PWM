/****************************************************************************************************************************
  megaAVR_SLOW_PWM.h
  For Arduino megaAVR ATMEGA4809-based boards (UNO WiFi Rev2, NANO_EVERY, etc. )
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/megaAVR_SLOW_PWM
  Licensed under MIT license
  
  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one megaAVR-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.1.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      27/09/2021 Initial coding for megaAVR-based boards (UNO WiFi Rev2, NANO_EVERY, etc.)
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
*****************************************************************************************************************************/

#pragma once

#ifndef MEGA_AVR_SLOW_PWM_H
#define MEGA_AVR_SLOW_PWM_H

#if defined(BOARD_NAME)
  #undef BOARD_NAME
#endif

#if ( defined(__AVR_ATmega4809__) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_AVR_NANO_EVERY) )
  #if !defined(BOARD_NAME)
    #if (ARDUINO_AVR_UNO_WIFI_REV2)
      #define BOARD_NAME      "megaAVR UNO WiFi Rev2"
    #elif (ARDUINO_AVR_NANO_EVERY)
      #define BOARD_NAME      "megaAVR Nano Every"
    #else
      #define BOARD_NAME      "megaAVR Unknown"
    #endif
  #endif
#else
  #error This is designed only for Arduino megaAVR board! Please check your Tools->Board setting.
#endif

#ifndef MEGA_AVR_SLOW_PWM_VERSION
  #define MEGA_AVR_SLOW_PWM_VERSION       F("megaAVR_SLOW_PWM v1.1.0")
#endif

#ifndef _PWM_LOGLEVEL_
  #define _PWM_LOGLEVEL_        1
#endif

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Arduino.h"
#include "pins_arduino.h"

#include "PWM_Generic_Debug.h"

#define MAX_COUNT_16BIT           65535

typedef void (*timer_callback)(void);
typedef void (*timer_callback_p)(void *);

// Count only TCB0-TCB3
enum
{
  HW_TIMER_0 = 0,
  HW_TIMER_1,
  HW_TIMER_2,
  HW_TIMER_3,
  NUM_HW_TIMERS
};


class TimerInterrupt
{
  private:

    bool            _timerDone;
    int8_t          _timer;
    unsigned int    _prescalerIndex;
    uint32_t        _CCMPValue;
    uint32_t        _CCMPValueRemaining;
    volatile long   _toggle_count;
    double           _frequency;

    void*           _callback;        // pointer to the callback function
    void*           _params;          // function parameter

    void set_CCMP(void);

  public:

    TimerInterrupt()
    {
      _timer              = -1;
      _frequency          = 0;
      _callback           = NULL;
      _params             = NULL;
      _timerDone          = false;
      _CCMPValue           = 0;
      _CCMPValueRemaining  = 0;
      _toggle_count       = -1;
    };

    explicit TimerInterrupt(uint8_t timerNo)
    {
      _timer              = timerNo;
      _frequency          = 0;
      _callback           = NULL;
      _params             = NULL;
      _timerDone          = false;
      _CCMPValue           = 0;
      _CCMPValueRemaining  = 0;
      _toggle_count       = -1;
    };

    void callback() __attribute__((always_inline))
    {
      if (_callback != NULL)
      {
        if (_params != NULL)
          (*(timer_callback_p)_callback)(_params);
        else
          (*(timer_callback)_callback)();
      }
    }

    void init(int8_t timer);

    void init()
    {
      init(_timer);
    };

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    bool setFrequency(float frequency, timer_callback_p callback, /* void* */ uint32_t params, unsigned long duration = 0);

    // frequency (in hertz) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    bool setFrequency(float frequency, timer_callback callback, unsigned long duration = 0)
    {
      return setFrequency(frequency, reinterpret_cast<timer_callback_p>(callback), /*NULL*/ 0, duration);
    }

    // interval (in ms) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    template<typename TArg>
    bool setInterval(unsigned long interval, void (*callback)(TArg), TArg params, unsigned long duration = 0)
    {
      static_assert(sizeof(TArg) <= sizeof(uint32_t), "setInterval() callback argument size must be <= 4 bytes");
      return setFrequency((float) (1000.0f / interval), reinterpret_cast<timer_callback_p>(callback), (uint32_t) params, duration);
    }

    // interval (in ms) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    bool setInterval(unsigned long interval, timer_callback callback, unsigned long duration = 0)
    {
      return setFrequency((float) (1000.0f / interval), reinterpret_cast<timer_callback_p>(callback), /*NULL*/ 0, duration);
    }

    template<typename TArg>
    bool attachInterrupt(float frequency, void (*callback)(TArg), TArg params, unsigned long duration = 0)
    {
      static_assert(sizeof(TArg) <= sizeof(uint32_t), "attachInterrupt() callback argument size must be <= 4 bytes");
      return setFrequency(frequency, reinterpret_cast<timer_callback_p>(callback), (uint32_t) params, duration);
    }

    bool attachInterrupt(float frequency, timer_callback callback, unsigned long duration = 0)
    {
      return setFrequency(frequency, reinterpret_cast<timer_callback_p>(callback), /*NULL*/ 0, duration);
    }

    // Interval (in ms) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    template<typename TArg>
    bool attachInterruptInterval(float interval, void (*callback)(TArg), TArg params, unsigned long duration = 0)
    {
      static_assert(sizeof(TArg) <= sizeof(uint32_t), "attachInterruptInterval() callback argument size must be <= 4 bytes");
      return setFrequency( (float) ( 1000.0f / interval), reinterpret_cast<timer_callback_p>(callback), (uint32_t) params, duration);
    }

    // Interval (in ms) and duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    bool attachInterruptInterval(float interval, timer_callback callback, unsigned long duration = 0)
    {
      return setFrequency( (float) ( 1000.0f / interval), reinterpret_cast<timer_callback_p> (callback), /*NULL*/ 0, duration);
    }

    void detachInterrupt();

    void disableTimer(void)
    {
      detachInterrupt();
    }

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void reattachInterrupt(unsigned long duration = 0);

    // Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
    void enableTimer(unsigned long duration = 0) __attribute__((always_inline))
    {
      reattachInterrupt(duration);
    }

    // Just stop clock source, still keep the count
    void pauseTimer(void);

    // Just reconnect clock source, continue from the current count
    void resumeTimer(void);

    // Just stop clock source, clear the count
    void stopTimer(void)
    {
      detachInterrupt();
    }

    // Just reconnect clock source, start current count from 0
    void restartTimer(unsigned long duration = 0)
    {
      reattachInterrupt(duration);
    }

    int8_t getTimer() __attribute__((always_inline))
    {
      return _timer;
    };

    long getCount() __attribute__((always_inline))
    {
      return _toggle_count;
    };

    void setCount(long countInput) __attribute__((always_inline))
    {
      //noInterrupts();

      _toggle_count = countInput;

      //interrupts();
    };

    long get_CCMPValue() __attribute__((always_inline))
    {
      return _CCMPValue;
    };

    long get_CCMPValueRemaining() __attribute__((always_inline))
    {
      return _CCMPValueRemaining;
    };

    void adjust_CCMPValue() //__attribute__((always_inline))
    {
      noInterrupts();

      _CCMPValueRemaining -= min(MAX_COUNT_16BIT, _CCMPValueRemaining);

      if (_CCMPValueRemaining == 0)
      {
        // Reset value for next cycle
        _CCMPValueRemaining = _CCMPValue;
        PWM_LOGDEBUG1(F("adjust_CCMPValue: reset _CCMPValueRemaining = "), _CCMPValue);
        _timerDone = true;
      }
      else
        _timerDone = false;

      interrupts();
    };

    void reload_CCMPValue() //__attribute__((always_inline))
    {
      noInterrupts();

      // Reset value for next cycle, have to deduct the value already loaded to CCMP register

      _CCMPValueRemaining -= min(MAX_COUNT_16BIT, _CCMPValueRemaining);

      _timerDone = false;

      interrupts();
    };

    bool checkTimerDone(void) //__attribute__((always_inline))
    {
      return _timerDone;
    };

}; // class TimerInterrupt

//////////////////////////////////////////////

// To be sure not used Timers are disabled

// TCB0
#if !defined(USE_TIMER_0)
  #define USE_TIMER_0     false
#endif

// TCB1
#if !defined(USE_TIMER_1)
  #define USE_TIMER_1     false
#endif

// TCB2
#if !defined(USE_TIMER_2)
  #define USE_TIMER_2     false
#endif

// TCB3
#if !defined(USE_TIMER_3)
  #define USE_TIMER_3     false
#endif


//////////////////////////////////////////////

#if USE_TIMER_0
  #ifndef TIMER0_INSTANTIATED
    // To force pre-instatiate only once
    #define TIMER0_INSTANTIATED
    TimerInterrupt ITimer0(HW_TIMER_0);
    
    ISR(TCB0_INT_vect)
    {
      long countLocal = ITimer0.getCount();
              
      if (ITimer0.getTimer() == 0)
      {
        if (countLocal != 0)
        {
          if (ITimer0.checkTimerDone())
          {  
            PWM_LOGDEBUG3((F("T0 callback, _CCMPValueRemaining =")), ITimer0.get_CCMPValueRemaining(), (F(", millis =")), millis());
            
            ITimer0.callback();
            
            // To reload _CCMPValueRemaining as well as _CCMP register to MAX_COUNT_16BIT
            ITimer0.reload_CCMPValue();
            
            if (countLocal > 0)
              ITimer0.setCount(countLocal - 1);       
          }
          else
          {
            //Deduct _CCMPValue by min(MAX_COUNT_16BIT, _CCMPValue)
            // If _CCMPValue == 0, flag _timerDone for next cycle     
            // If last one (_CCMPValueRemaining < MAX_COUNT_16BIT) => load _CCMP register _CCMPValueRemaining
            ITimer0.adjust_CCMPValue();
          }
        }
        else
        {
          PWM_LOGWARN((F("T0 done")));
          
          ITimer0.detachInterrupt();
        }
      }
      
      // Clear interrupt flag
      TCB0.INTFLAGS = TCB_CAPT_bm;
    }
  #endif  //#ifndef TIMER0_INSTANTIATED
#endif    //#if USE_TIMER_0

#if USE_TIMER_1 
#ifndef TIMER1_INSTANTIATED
  // To force pre-instatiate only once
  #define TIMER1_INSTANTIATED
  TimerInterrupt ITimer1(HW_TIMER_1);
  
  // Timer0 is used for micros(), millis(), delay(), etc and can't be used
  // Pre-instatiate

  ISR(TCB1_INT_vect)
  {
    long countLocal = ITimer1.getCount();
         
    if (ITimer1.getTimer() == 1)
    {
      if (countLocal != 0)
      {
        if (ITimer1.checkTimerDone())
        {
          PWM_LOGDEBUG3((F("T1 callback, _CCMPValueRemaining =")), ITimer1.get_CCMPValueRemaining(), (F(", millis =")), millis());
          
          ITimer1.callback();
          
          // To reload _CCMPValueRemaining as well as _CCMP register to MAX_COUNT_16BIT if _CCMPValueRemaining > MAX_COUNT_16BIT
          ITimer1.reload_CCMPValue();
               
          if (countLocal > 0)                  
            ITimer1.setCount(countLocal - 1);
        }
        else
        {
          //Deduct _CCMPValue by min(MAX_COUNT_16BIT, _CCMPValue)
          // If _CCMPValue == 0, flag _timerDone for next cycle  
          // If last one (_CCMPValueRemaining < MAX_COUNT_16BIT) => load _CCMP register _CCMPValueRemaining
          ITimer1.adjust_CCMPValue();
        }         
      }
      else
      {
        PWM_LOGWARN((F("T1 done")));
        
        ITimer1.detachInterrupt();
      }
    }
    
    // Clear interrupt flag
    TCB1.INTFLAGS = TCB_CAPT_bm;
  }
  
#endif  //#ifndef TIMER1_INSTANTIATED
#endif    //#if USE_TIMER_1

#if USE_TIMER_2
#ifndef TIMER2_INSTANTIATED
  #define TIMER2_INSTANTIATED
  TimerInterrupt ITimer2(HW_TIMER_2);
  
  ISR(TCB2_INT_vect)
  {
    long countLocal = ITimer2.getCount();
   
    if (ITimer2.getTimer() == 2)
    {
      if (countLocal != 0)
      {
        if (ITimer2.checkTimerDone())
        {
          PWM_LOGDEBUG3((F("T2 callback, _CCMPValueRemaining =")), ITimer2.get_CCMPValueRemaining(), (F(", millis =")), millis());
           
          ITimer2.callback();
          // To reload _CCMPValue
          ITimer2.reload_CCMPValue();

          if (countLocal > 0)
           ITimer2.setCount(countLocal - 1);

        }
        else
        {           
          //Deduct _CCMPValue by min(MAX_COUNT_8BIT, _CCMPValue)
          // If _CCMPValue == 0, flag _timerDone for next cycle
          ITimer2.adjust_CCMPValue();
        }          
      }    
      else
      {
        PWM_LOGWARN((F("T2 done")));
        
        ITimer2.detachInterrupt();
      }
    }
    
    // Clear interrupt flag
    TCB2.INTFLAGS = TCB_CAPT_bm;
  }    
#endif  //#ifndef TIMER2_INSTANTIATED
#endif    //#if USE_TIMER_2

// Pre-instatiate
#if USE_TIMER_3
  #ifndef TIMER3_INSTANTIATED
    // To force pre-instatiate only once
    #define TIMER3_INSTANTIATED
    TimerInterrupt ITimer3(HW_TIMER_3);
    
    ISR(TCB3_INT_vect)
    {
      long countLocal = ITimer3.getCount();
      
      if (ITimer3.getTimer() == 3)
      {
        if (countLocal != 0)
        {
          if (ITimer3.checkTimerDone())
          { 
            PWM_LOGDEBUG3((F("T3 callback, _CCMPValueRemaining =")), ITimer3.get_CCMPValueRemaining(), (F(", millis =")), millis());
            
            ITimer3.callback();
            
            // To reload _CCMPValueRemaining as well as _CCMP register to MAX_COUNT_16BIT
            ITimer3.reload_CCMPValue();
            
            if (countLocal > 0)
              ITimer3.setCount(countLocal - 1);     
          }
          else
          {
            //Deduct _CCMPValue by min(MAX_COUNT_16BIT, _CCMPValue)
            // If _CCMPValue == 0, flag _timerDone for next cycle          
            // If last one (_CCMPValueRemaining < MAX_COUNT_16BIT) => load _CCMP register _CCMPValueRemaining
            ITimer3.adjust_CCMPValue();
          }
        }
        else
        {
          PWM_LOGWARN((F("T3 done")));
          
          ITimer3.detachInterrupt();
        }
      }
      
      // Clear interrupt flag
      TCB3.INTFLAGS = TCB_CAPT_bm;
    }  
    
  #endif  //#ifndef TIMER3_INSTANTIATED
#endif    //#if USE_TIMER_3


#include "megaAVR_Slow_PWM_ISR.h"


#endif    // MEGA_AVR_SLOW_PWM_H

