#pragma once

#include "Arduino.h"
#include "arm_math.h" // __LDREXW
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
        uint32_t monitor; // used to monitor if an interrupt occured in the critical section
        uint64_t curVal;

        do { // interrupts between __LDREXW and _STREXW make the STREXW call failing, in this case we recalculate (see https://community.arm.com/support-forums/f/architectures-and-processors-forum/10361/ldrex-strex-on-the-m3-m4-m7/33045#33045)
            __LDREXW(&monitor);

            uint32_t curLow = ARM_DWT_CYCCNT;
            if (curLow < oldLow) // roll over
            {
                curHigh++;
            }
            oldLow = curLow;
            curVal = ((uint64_t)curHigh << 32) | curLow;

        } while (__STREXW(1, &monitor)); // returns != 0 if an interrupt occurred in the block

        return curVal;
    }

    static tickType getRTC()
    {
        return (tickType)rtc_get();
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