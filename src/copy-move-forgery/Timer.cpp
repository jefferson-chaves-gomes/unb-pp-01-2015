
#include "Timer.h"

#ifdef MPI_ENABLED
#include <MPISettings.h>
#endif

Timer::Timer(bool print_, std::string const& function_, int line_, std::string const& message_) :
    print(print_),
    meter(false),
    function(function_),
    line(line_),
    message(message_),
    startTime(boost::posix_time::microsec_clock::local_time()
                    .time_of_day()
                    .total_microseconds())
{
}

Timer::Timer(bool meter_, std::string const& message_) :
    print(false),
    meter(meter_),
    function(""),
    line(0),
    message(message_),
    startTime(boost::posix_time::microsec_clock::local_time()
                    .time_of_day()
                    .total_microseconds())
{
}

Timer::Timer() :
    print(false),
    meter(false),
    line(0),
    message(""),
    startTime(boost::posix_time::microsec_clock::local_time()
                    .time_of_day()
                    .total_microseconds())
{
}

Timer::~Timer()
{
#ifdef MPI_ENABLED
    if(MPISettings::PROC_ID() != 0)
        return;
#endif
    if(print)
        std::cout << "[" << elapsedMicroseconds() << "] " << function << ":" << line << (message.empty() ? "" : " => ")  << message << std::endl;
    if (meter)
        std::cout << "[" << elapsedMicroseconds() << "]\t" << message << std::endl;
}

long long Timer::elapsedMicroseconds()
{
    return (boost::posix_time::microsec_clock::local_time()
                    .time_of_day()
                    .total_microseconds()
                - startTime);
}
