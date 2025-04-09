#include"TcpConnection.h"
using namespace ilib::net;

void TcpConnection::handleRead() {
    char buff[65535];
    ssize_t n = ::read(channel_->fd(),buff,sizeof(buff));
    // messageCallback_(shared_from_this(), buff,n);
}