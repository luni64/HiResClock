#include "Arduino.h"
#include "HiResClock.h"
#include <chrono>

// PJRC Teensy boards ==========================================
#if defined(TEENSYDUINO)

#if defined(__IMXRT1062__)                // Teensy 4X and Teensy MM
#include "clockSources/T4xSource.h"
using cppClock = HiResClock<T4X_Source>;

#elif defined(KINETISK)                   //Teensy 3X
#include "clockSources/T3xSource.h"
using cppClock = HiResClock<T3X_Source>;

#elif defined(KINETISL)                   //Teensy LC
#error "Doesn't support TeensyLC"
#endif

//STM boards ===================================================
//unfortunately F_CPU is not a compile-time constant in STM32duino.
//Thus, we need to provide the clock frequency manually for the
// various boards.
#elif defined(ARDUINO_ARCH_STM32)
    #if defined(STM32F4xx)
        #include "clockSources/STM32F4xx.h"
        #include "STM32RTC.h"
        #if defined(STM32F401xE)
            using cppClock = HiResClock<TickSource_t<84'000'000>>;
        #elif defined(STM32F411xE)
            using cppClock = HiResClock<TickSource_t<100'000'000>>;
        #endif
    #endif
#endif