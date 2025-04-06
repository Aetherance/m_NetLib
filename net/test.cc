#include"Acceptor.h"
#include"InetAddress.h"
#include"EventLoop.h"

using namespace ilib;

void newConnection(int sock,const net::InetAddress & peerAddr) {
    printf("new connection: accepted a new connection\n");
    send(sock,"Hello Peer!",12,0);
    net::socket::close(sock);
}

int main() {
    net::InetAddress listenAddr(9981);
    net::EventLoop loop;

    net::Acceptor acceptor(&loop,listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();

    return 0;
}