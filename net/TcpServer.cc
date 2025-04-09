#include"../base/logger.h"
#include"TcpServer.h"
using namespace ilib::net;

void TcpServer::newConnection(int sockfd,const InetAddress& peerAddr) {
    loop_->assertInLoopThread();
    char buff[32];
    snprintf(buff,sizeof(buff),"#%d",nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buff;
    LOG_CLIENT_INFO(CONNECT_ON,sockfd);

    InetAddress localAddr(socket::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_,connName,sockfd,localAddr,peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->connectEstablished();
}