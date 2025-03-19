#include"EventLoop.h"
#include<iostream>
#include<poll.h>

using namespace ilib::net;

thread_local EventLoop * t_loopInThisThread = nullptr;

EventLoop::EventLoop() : 
looping_(false) , threadid_(std::this_thread::get_id()) {
    
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

    ::poll(nullptr,0,5000);

    looping_ = false;
}

void EventLoop::assertInLoopThread() {
    if(!isInLoopThread()) {
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread() {
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread() {
    // abort
    std::cerr<<"Eventloop: Not in loop thread";
    exit(EXIT_FAILURE);
}