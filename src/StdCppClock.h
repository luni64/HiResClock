#pragma once

#include <chrono>

template <typename TickSource>
struct StdCppClock
{
    // required typedefs:
    using duration   = typename TickSource::tickDuration; // use a uint64_t representation with a time step of 1/F_CPU (=1.667ns @600MHz)
    using rep        = typename duration::rep;            // uint64_t
    using period     = typename duration::period;         // std::ratio<1,600'000'000>
    using time_point = std::chrono::time_point<StdCppClock, duration>;

    static constexpr bool is_steady = false; // can not be guaranteed to be steady (could be readjusted by syncToRTC)

    static time_point now()
    {
        duration t = duration(t0 + TickSource::getTicks()); // adds the current 64bit cycle counter to an offset set by syncToRTC() (default: t0=0)
        return time_point(t);                               // ... and returns the corresponding time point.
    }

    static void begin(bool sync = true); // starts the 64bit cycle counter update interrupt. Sync=true syncs the clock to the RTC
    static void syncToRTC();             // Sync to RTC whenever needed (e.g. after adjusting the RTC)

    // Map to C API
    static std::time_t to_time_t(const time_point& t); // returns the time_t value (seconds since 1.1.1970) to be used with standard C-API functions
    static time_point from_time_t(std::time_t t);      // converts a time_t value to a time_point

 private:
    static rep t0; // offset to adjust time (seconds from 1.1.1970 to now).
};


// Inline implementation =====================================================================

template <typename TickSource>
void StdCppClock<TickSource>::begin(bool sync)
{
   TickSource::begin();
    if (sync) syncToRTC();
}

template <typename TickSource>
std::time_t StdCppClock<TickSource>::to_time_t(const time_point& t)
{
    using namespace std::chrono;
    return std::time_t(duration_cast<seconds>(t.time_since_epoch()).count());
}

template <typename TickSource>
typename StdCppClock<TickSource>::time_point StdCppClock<TickSource>::from_time_t(std::time_t t)
{
    using namespace std::chrono;
    typedef std::chrono::time_point<StdCppClock, seconds> from;
    return time_point_cast<duration>(from(seconds(t)));
}

template <typename TickSource>
void StdCppClock<TickSource>::syncToRTC()
{
   // t0 = ((rep)rtc_get()) * F_CPU - TickSource::getTicks();
}

template <typename TickSource>
typename StdCppClock<TickSource>::rep StdCppClock<TickSource>::t0 = 0;
