#include"TcpConnection.h"
#include"../base/logger.h"
using namespace ilib::net;

TcpConnection::
TcpConnection(EventLoop * loop,
              std::string name,
              int sockfd,const InetAddress localAddr,
              const InetAddress peerAddr) 
              : loop_(loop),
                name_(name),
                state_(kConnecting),
                socket_(new Socket(sockfd)),
                channel_(new Channel(loop,sockfd)),
                localAddr_(localAddr),
                peerAddr_(peerAddr) 
{
    LOG_CLIENT_INFO(CONNECT_ON,sockfd);
    // channel_->setReadCallback([this](){handleRead();});
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(),&saveErrno);

    if(n > 0) {
        messageCallback_(shared_from_this(),&inputBuffer_,receiveTime);
    } else if(n == 0) {
        handleClose();
    } else {
        errno = saveErrno;
        LOG_ERROR("TcpConnect");
        handleError();
    }
}