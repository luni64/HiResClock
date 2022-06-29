#include "Arduino.h"
#include "HiResClock.h"
#include "cycles64.h"

void HiResClock::begin(bool sync)
{
    cycles64::begin();
    if (sync) syncToRTC();
}

std::time_t HiResClock::to_time_t(const time_point& t)
{
    using namespace std::chrono;
    return std::time_t(duration_cast<seconds>(t.time_since_epoch()).count());
}

HiResClock::time_point HiResClock::from_time_t(std::time_t t)
{
    using namespace std::chrono;
    typedef std::chrono::time_point<HiResClock, seconds> from;
    return time_point_cast<duration>(from(seconds(t)));
}

void HiResClock::syncToRTC()
{
    t0 = ((uint64_t)rtc_get()) * F_CPU - cycles64::get();
}

uint64_t HiResClock::t0 = 0;
