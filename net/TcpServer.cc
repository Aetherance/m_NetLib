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

    EventLoop * subLoop = threadpool_->getNextLoop();

    InetAddress localAddr(socket::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(subLoop,connName,sockfd,localAddr,peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback([this](const TcpConnectionPtr & conn){ removeConnection(conn); });
    subLoop->runInloop([conn]{ conn->connectEstablished(); }); // ??
}

TcpServer::TcpServer(EventLoop * loop,const InetAddress & listenAddr) 
         : loop_(loop),
           acceptor_(new Acceptor(loop,listenAddr)),
           threadpool_(new EventLoopThreadPool(loop)),
           connectionCallback_(),
           messageCallback_(),
           started_(false),
           nextConnId_(1)
{
    acceptor_->setNewConnectionCallback([this](int sockfd,const InetAddress &peerAddr){ newConnection(sockfd,peerAddr); });
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {

}

void TcpServer::removeConnection(const TcpConnectionPtr & conn) {

}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr & conn) {
    
}