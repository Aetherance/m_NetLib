#include"TcpConnection.h"
#include"../base/logger.h"
#include<assert.h>

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
    channel_->setReadCallback([this](Timestamp recvTime){ handleRead(recvTime);});
    channel_->setWriteCallback([this]{ handleWrite(); });
    channel_->setCloseCallback([this]{ handleClose(); });
    channel_->setErrorCallback([this]{ handleError(); });
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

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if(channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(),outputBuffer_.peek(),outputBuffer_.readableBytes());
        if(n > 0) {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if(writeCompleteCallback_) {
                    loop_->queueInLoop([this](){ writeCompleteCallback_(shared_from_this()); });
                }
                if(state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        }
    }
}

void TcpConnection::handleError() {
    LOG_CLIENT_ERROR("failed",channel_->fd());   
}

void TcpConnection::shutdown() {
    if(state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInloop([this](){ shutdownInLoop(); });
    }
}

void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    socket_->shutdownWrite();
}

void TcpConnection::send(const std::string & msg) {
    if(state_ == kConnected) {
        if(loop_->isInLoopThread()) {
            sendInLoop(msg);
        } else {
            loop_->runInloop([this,&msg]{ sendInLoop(std::move(msg)); });
        }
    }
}

void TcpConnection::sendInLoop(const std::string & msg) {
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = ::write(channel_->fd(),msg.data(),msg.size());
        if(nwrote >= 0) {
            if(static_cast<size_t>(nwrote) < msg.size()) {
                // 不完全写入...
                LOG_WARN("data pending");
            } else if(writeCompleteCallback_){
                // 完全写入...
                loop_->queueInLoop([this](){ writeCompleteCallback_(shared_from_this()); });
            }
        } else {
            // 写入失败...
            nwrote = 0;
            if(errno != EWOULDBLOCK) {
                LOG_FATAL("sendInLoop: send failed!");
            }
        }
    }

    assert(nwrote >= 0);

    // 处理未发送数据...
    if(static_cast<size_t>(nwrote) < msg.size()) {
        outputBuffer_.append(msg.data() + nwrote,msg.size() - nwrote);
        if(!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();
    // LOG_INFO("A connection has established!");
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();

    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());

    loop_->removeChannel(channel_.get());
}