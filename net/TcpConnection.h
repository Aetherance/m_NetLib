#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include"../base/noncopyable.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"Socket.h"

namespace ilib{
namespace net{

class Buffer;
class TcpConnection : noncopyable , public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

    TcpConnection(EventLoop * loop,std::string connName,int sockfd,InetAddress localAddr,InetAddress peerAddr);

    EventLoop * getLoop() const;
    const std::string & name() const;
    const InetAddress & localAddress() const;
    const InetAddress & peerAddress() const;
    bool connectd() const;

    void setConnectionCallback(ConnectionCallback);
    void setMessageCallback(MessageCallback);
    
    void connectEstablished();
    void connectDestroyed();

    void send(const std::string& message);

    void shutdown();
    void setTcpNoDelay(bool on);

private:
    enum StateE { kConnecting, kConnected , kDisconnecting , kDisconnected };

    void setState(StateE s) { state_ = s; };
    void handleRead(Timestamp receiveTime);
    void handleClose();
    void handleWrite();
    void handleError();
    void sendInLoop(const std::string & msg);
    void shutdownInLoop();

    EventLoop * loop_;
    std::string name_;
    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

};

}
}


#endif