
#include "Arduino.h"
#include "HiResClock.h"
#include "time.h"
using namespace std::chrono;

typedef HiResClock::time_point timePoint; // just for the sake of less typing

void setup()
{
    HiResClock::begin(); // t_0 the clock and sync to rtc (works with and without battery)
}


void loop()
{
    // demonstrate clock resolution -------------------------------------------------------------------------------
    unsigned delay_us = random(10, 5000);
    Serial.printf("delayMicroseconds(%u)\n", delay_us);

    timePoint start = HiResClock::now();                 // get two time-points 'delay_us' apart
    delayMicroseconds(delay_us);
    timePoint stop = HiResClock::now();
    auto delta = stop - start;                           // calculate time difference before and after the delay

    // convert dt to nanoseconds
    nanoseconds ns = duration_cast<nanoseconds>(delta);  // convert to nanosecond ticks
    Serial.printf("dt = t_1 - t_0: %ull ns\n", ns.count());

    // convert "start" to C-API time_t  ----------------------------------------------------------------------------
    time_t ct = HiResClock::to_time_t(start);          // convert chrono::timePoint to C-API time_t
    Serial.printf("Current Time: %s\n", ctime(&ct));   // pretty print date/time

    delay(1000);
}