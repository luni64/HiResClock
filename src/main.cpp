
#include "Arduino.h"
#include "cppClock.h"


using namespace std::chrono;


void setup()
{
    Serial.begin(9600);
    cppClock::begin(); // t_0 the clock and sync to rtc (works with and without battery)

    
}

void loop()
{
    // demonstrate clock resolution -------------------------------------------------------------------------------
    unsigned delay_us = random(10, 5000);
    Serial.printf("delayMicroseconds(%u)\n", delay_us);

    auto start = cppClock::now(); // get two time-points 'delay_us' apart
    delayMicroseconds(delay_us);
    auto stop = cppClock::now();
    auto delta     = stop - start; // calculate time difference before and after the delay

    // convert dt to nanoseconds
    nanoseconds ns = duration_cast<nanoseconds>(delta); // convert to nanosecond ticks
    Serial.printf("dt = t_1 - t_0: %ull ns\n", ns.count());

    // convert "start" to C-API time_t  ----------------------------------------------------------------------------
    time_t ct = cppClock::to_time_t(start);        // convert chrono::timePoint to C-API time_t
    Serial.printf("Current Time: %s\n", ctime(&ct)); // pretty print date/time

    delay(3000);
}