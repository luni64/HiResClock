#pragma once

#include "Arduino.h"
#include <chrono>
#include "STM32RTC.h"


template <unsigned tickFrequency>
class TickSource_t
{
    using tickType  = uint64_t;
    using tickRatio = std::ratio<1, tickFrequency>;

 public:
    using tickDuration = std::chrono::duration<tickType, tickRatio>;

    static void begin()
    {
        curHigh = 0;
        oldLow  = DWT->CYCCNT;
        STM32RTC& rtc = STM32RTC::getInstance();
        rtc.begin(HOUR_FORMAT_24);
        rtc.attachSecondsInterrupt([](void*) { getTicks(); });
    }

    static tickType getTicks()
    {
        noInterrupts();
        uint32_t curLow = DWT->CYCCNT;
        if (curLow < oldLow) // roll over
        {
            curHigh++;
        }
        oldLow          = curLow;
        uint64_t curVal = ((uint64_t)curHigh << 32) | curLow;
        interrupts();

        return curVal;
    }

 protected:
    static uint32_t oldLow;
    static uint32_t curHigh;
};


template <unsigned tickFrequency>
uint32_t TickSource_t<tickFrequency>::curHigh;
template <unsigned tickFrequency>
uint32_t TickSource_t<tickFrequency>::oldLow;