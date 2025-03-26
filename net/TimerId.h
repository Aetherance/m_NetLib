#ifndef TIMERID_H
#define TIMERID_H

#include<cstdint>

namespace ilib {
namespace net {

class Timer;
class TimerId
{
public:
    TimerId(Timer * timer,int64_t seq = 0);
    friend class TimerQueue;
private:
    Timer *timer_;
    int64_t seq_;
};

inline TimerId::TimerId(Timer * timer,int64_t seq = 0) :
timer_(timer) , seq_(seq)
{}

}    
}

#endif