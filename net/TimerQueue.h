#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include"../base/Timestamp.h"
#include"../base/noncopyable.h"
#include"Timer.h"
#include<set>
#include<vector>
#include<Channel.h>
#include<memory>

namespace ilib {
namespace net {

class EventLoop;
class TimerQueue : noncopyable
{
public:
    TimerQueue(EventLoop * loop);
    ~TimerQueue();
    TimerId addTimer(const Timer::TimerCallback &cb,Timestamp when,double interval);
    void cancel();
private:
    using Entry = std::pair<base::Timestamp,Timer*>;
    using TimerList = std::set<Entry>;

    void addTimerInLoop(Timer * timer);
    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired,Timestamp now);

    bool insert(Timer * timer);

    EventLoop * loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    TimerList timers_;
};

} 
}

#endif