#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<thread>
#include<memory>

namespace ilib {
namespace net {

class EventLoop
{
public:
EventLoop();
    EventLoop(EventLoop &) = delete;
    ~EventLoop();
    
    void loop();
    
    void assertInLoopThread();
    bool isInLoopThread();
    static EventLoop * getThreadLoop();
    
    private:
    void abortNotInLoopThread();
    
    bool looping_;
    const std::thread::id threadid_;
};

}
}

#endif