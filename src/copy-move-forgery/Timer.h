#ifndef TIMER_H_
#define TIMER_H_

#include <boost/date_time/posix_time/posix_time.hpp>

class Timer
{
    bool print;
    const std::string function;
    int line;
    const std::string message;
    long long startTime;

public:
    Timer(bool print_, std::string const& function_, int line_, std::string const& message_ = "");
    Timer(bool print_, std::string const& function_);
    Timer();
    ~Timer();
    long long elapsedMicroseconds();
};

#endif
