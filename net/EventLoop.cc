#include"EventLoop.h"
#include<iostream>
#include<poll.h>
#include"Channel.h"
#include"Poller.h"

using namespace ilib::net;

thread_local EventLoop * t_loopInThisThread = nullptr;

EventLoop::EventLoop() : 
looping_(false) , threadid_(std::this_thread::get_id()) , poller_(std::make_unique<Poller>(this)) {
    
    // 已经存在Eventloop对象
    if(t_loopInThisThread) {
        std::cerr<<"Eventloop: loop has been created\n";
        exit(EXIT_FAILURE);
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    
    // 析构时不能正在循环
    if(looping_) {
        std::cerr<<"Eventloop: ~ while looping\n";
        exit(EXIT_FAILURE);
    }
    t_loopInThisThread = nullptr;
}

EventLoop* EventLoop::getThreadLoop() {
    return t_loopInThisThread;
}

void EventLoop::loop() {
    if(looping_) {
        std::cerr<<"Eventloop: looping\n";
        exit(EXIT_FAILURE);
    }
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(-1,activeChannels_);
        for(auto channel : activeChannels_) {
            channel->handleEvent();
        }
        doPendingFunctors();
    }

    looping_ = false;
}

void EventLoop::assertInLoopThread() {
    if(!isInLoopThread()) {
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread() const {
    return threadid_ == std::this_thread::get_id();
}

void EventLoop::abortNotInLoopThread() {
    // abort
    std::cerr<<"Eventloop: Not in loop thread";
    exit(EXIT_FAILURE);
}

void EventLoop::updateChannel(Channel * channel) {
    poller_->updateChannel(channel);
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}  

TimerId EventLoop::runAt(const Timestamp &time,const Timer::TimerCallback &cb) {
    return timerqueue_->addTimer(cb,time,0);
}

TimerId EventLoop::runAfter(double delay,const Timer::TimerCallback &cb) {
    Timestamp time(Timestamp::addTime(Timestamp::now(),delay));
    return runAt(time,cb);
}

TimerId EventLoop::runEvery(double interval,const Timer::TimerCallback &cb) {
    Timestamp time(Timestamp::addTime(Timestamp::now(),interval));
    return timerqueue_->addTimer(cb,time,interval);
}

void EventLoop::runInloop(const Functor&cb) {
    if(isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor &cb) {
    {
        std::unique_lock(mtx);
        pendingFunctors_.push_back(cb);
    }

    if(!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor>functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock lock(mtx);
        functors.swap(pendingFunctors_);
    }

    for(auto functor : functors) {
        functor();
    }

    callingPendingFunctors_ = false;
}