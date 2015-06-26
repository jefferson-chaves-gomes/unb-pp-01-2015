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

    Timer(bool print_, std::string const& function_, int line_, std::string const& message_ = "") :
        print(print_),
        function(function_),
        line(line_),
        message(message_),
        startTime(boost::posix_time::microsec_clock::local_time()
                        .time_of_day()
                        .total_microseconds())
    {
    }

    Timer() :
        print(false),
        line(0),
        message(""),
        startTime(boost::posix_time::microsec_clock::local_time()
                        .time_of_day()
                        .total_microseconds())
    {
    }

    ~Timer()
    {
        if(print)
            std::cout << "[" << elapsedMicroseconds() << "] " << function << ":" << line << (message.empty() ? "" : " => ")  << message << std::endl;
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
