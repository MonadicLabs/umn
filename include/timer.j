#ifndef TIMER_H_
#define TIMER_H_

#include <ctime>
#include <chrono>

#define SECOND 1000000000

typedef std::chrono::high_resolution_clock clock_;

class Timer
{
public:
  //ctor that is probably never gonna be used
    inline Timer() {}

  //resets the starting time to the current time
    inline static void reset() { start = clock_::now(); }

  //returns the time elapsed from the starting time
    inline static long long elapsed() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clock_::now() - start).count();
    }

    inline static double elaspedMilliseconds() {
        long long nanos = elapsed();
        return (double)nanos / 1000000.0;
    }

    inline virtual ~Timer() {}

private:
    static std::chrono::time_point<clock_> start; //variable to store the starting time
};

#endif // TIMER_H_
