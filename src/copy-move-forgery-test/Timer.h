#ifndef TIMER_H_
#define TIMER_H_

#include <boost/date_time/posix_time/posix_time.hpp>

class Timer
{
    long long startTime;
public:
    Timer() :
        startTime(boost::posix_time::microsec_clock::local_time()
                        .time_of_day()
                        .total_microseconds())
    {
    }

    long long elapsedMicroseconds()
    {
        return (boost::posix_time::microsec_clock::local_time()
                        .time_of_day()
                        .total_microseconds()
                    - startTime);
    }
};

#endif
