#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<thread>
#include<memory>
#include"../base/noncopyable.h"

namespace ilib {
namespace net {

class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    
    void loop();
    
    void assertInLoopThread();
    bool isInLoopThread() const;
    static EventLoop * getThreadLoop();
    
    private:
    void abortNotInLoopThread();
    
    bool looping_;
    const std::thread::id threadid_;
};

}
}

#endif