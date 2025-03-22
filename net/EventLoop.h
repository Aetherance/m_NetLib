#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<thread>
#include<memory>
#include"../base/noncopyable.h"
#include<memory>
#include<vector>
#include"Poller.h"

namespace ilib {
namespace net {

class Channel;
class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    
    void loop();
    void quit();
    
    void assertInLoopThread();
    bool isInLoopThread() const;
    static EventLoop * getThreadLoop();
    void updateChannel(Channel * channel);
    
    private:
    void abortNotInLoopThread();

    using ChannelList = std::vector<Channel*>;
    
    bool quit_;
    bool looping_;
    const std::thread::id threadid_;
    std::unique_ptr<Poller>poller_;
    ChannelList activeChannels_;
};

}
}

#endif