#include"EventLoop.h"
#include<sys/timerfd.h>
#include"Poller.h"
#include"Channel.h"
using namespace ilib;

net::EventLoop * g_loop;

void timeout() {
    std::cout<<"时间到了!"<<std::endl;
    g_loop->quit();
}

int main() {
    net::EventLoop loop;
    g_loop = &loop;
    int timerfd = timerfd_create(CLOCK_MONOTONIC,0);
    net::Channel ch(&loop,timerfd);
    ch.setReadCallback(timeout);
    ch.enableReading();

    struct itimerspec time_long;
    time_long.it_interval.tv_sec = 0;
    time_long.it_value.tv_sec = 5;
    time_long.it_value.tv_nsec = 0;

    timerfd_settime(timerfd,0,&time_long,nullptr);
    
    loop.loop();

    return 0;
}