#ifndef TCPSERVER_H
#define TCPSERVER_H

#include"../base/noncopyable.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include<map>
#include"Acceptor.h"
#include"TcpConnection.h"

namespace ilib{
namespace net{

class TcpServer : noncopyable
{
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
    using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
    using ConnectionMap = std::map<std::string,TcpConnectionPtr>;
    using MessageCallback = std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)>;

public:
    TcpServer(EventLoop * loop,const InetAddress & listenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback & cb);
    void setMessageCallback(const MessageCallback & cb);
private:
    void newConnection(int sockfd,const InetAddress& peerAddr);


    EventLoop * loop_;
    const std::string name_;
    std::unique_ptr<Acceptor>acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    bool started_;
    int nextConnId_;
    ConnectionMap connections_;
};

}
}

#endif