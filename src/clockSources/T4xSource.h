#pragma once

#include "Arduino.h"
#include <chrono>

class T4X_Source
{
    using tickType  = uint64_t;
    using tickRatio = std::ratio<1, F_CPU>;

 public:
    using tickDuration = std::chrono::duration<tickType, tickRatio>;

    static void begin()
    {
        curHigh = 0;
        oldLow  = ARM_DWT_CYCCNT;
        setupOneSecTimer();
    }

    static tickType getTicks()
    {
        noInterrupts();
        uint32_t curLow = ARM_DWT_CYCCNT;
        if (curLow < oldLow) // roll over
        {
            curHigh++;
        }
        oldLow          = curLow;
        uint64_t curVal = ((uint64_t)curHigh << 32) | curLow;
        interrupts();

        return curVal;
    }

    static tickType getRTC()
    {
        return (tickType) rtc_get();
    }

 protected:
    static uint32_t oldLow;
    static uint32_t curHigh;

    static void setupOneSecTimer()
    {
        // disable periodic snvs interrupt
        SNVS_HPCR &= ~SNVS_HPCR_PI_EN;
        while ((SNVS_HPCR & SNVS_HPCR_PI_EN)) {}

        // interrupt frequency 1Hz
        SNVS_HPCR = SNVS_HPCR_PI_FREQ(0b1111);

        // enable periodic snvs interrupt
        SNVS_HPCR |= SNVS_HPCR_PI_EN;
        while (!(SNVS_HPCR & SNVS_HPCR_PI_EN)) {}

        attachInterruptVector(IRQ_SNVS_IRQ, SNVS_isr);
        NVIC_SET_PRIORITY(IRQ_SNVS_IRQ, 255); // lowest
        NVIC_ENABLE_IRQ(IRQ_SNVS_IRQ);
    }

    static void SNVS_isr(void)
    {
        SNVS_HPSR |= 0b11; // reset interrupt flag
        getTicks();        // call to check for overflow
        asm("dsb");        // prevent double calls of the isr
    }
};

uint32_t T4X_Source::oldLow;
uint32_t T4X_Source::curHigh;