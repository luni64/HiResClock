## High Resolution Clock for 32bit ARM boards (currently implemented for Teensy 4.1 only)

The clock uses the main clock of a T4.x (600MHz) as time-base for a `std::chrono` compatible clock. Resolution is 1/600MHz = 1.66ns.

By default `HiResClock::begin()` synchronizes the clock with the RTC.


A few useless usage examples

```c++
#include "HiResClock.h"

using namespace std::chrono;

void setup()
{
    HiResClock::begin(); // start the clock and sync to rtc (works with and without battery)

    auto t0 = HiResClock::now();
    delay(1234);
    auto t1 = HiResClock::now();

    nanoseconds delta_in_ns = duration_cast<nanoseconds>(t1 - t0);
    Serial.printf("delta: %0.9fs\n", delta_in_ns.count() / 1E9);

    // transform current time to standard time_t value
    auto now = HiResClock::now();
    time_t t = HiResClock::to_time_t(now);
    Serial.print(ctime(&t));
}

auto then = HiResClock::now() + 10h;

void loop()
{
    if (HiResClock::now() < then)
    {
        Serial.println("still waiting...");
    };

    constexpr unsigned delaytime = duration_cast<milliseconds>(0.34h + 2min + 3434ns).count(); // let the compiler calculate that
    delay(delaytime);
}
```