#include"EventLoop.h"
#include"TimerQueue.h"
#include"TimerId.h"
#include<algorithm>

using namespace ilib::net;

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<TimerQueue::Entry> ret_expired;
    Entry sentry = std::make_pair(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    std::copy(timers_.begin(),it,back_inserter(ret_expired));
    timers_.erase(timers_.begin(),it);
}

TimerId TimerQueue::addTimer(const Timer::TimerCallback &cb,Timestamp when,double interval) {
    Timer* timer = new Timer(cb,when,interval);
    loop_->runInloop([this,timer]{
        addTimerInLoop(timer);
    });
    return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer * timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if(earliestChanged) {
        resetTimerfd(timerfd_,timer->expiration());
    }
}