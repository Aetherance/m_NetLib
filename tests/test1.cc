// File: server_test.cpp
#include "../net/EventLoop.h"
#include "../net/TcpServer.h"
#include "../net/InetAddress.h"
#include <iostream>
#include <functional>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>

using namespace ilib;
using namespace ilib::net;
using namespace ilib::base;
using namespace std::placeholders;

// 简易Echo服务器
class EchoServer {
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr)
        : server_(loop, listenAddr) {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, _1, _2, _3));
    }

    void start() {
        server_.setThreadNum(32);
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        std::cout << "Server connection: "
                  << (conn->connected() ? "UP" : "DOWN") << std::endl;
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp) {
        std::string msg = buf->retrieveAsString();
        std::cout << "Server received: " << msg << std::endl;
        conn->send(msg);  // Echo back
    }

    TcpServer server_;
};

// 原生socket客户端验证
void test_client() {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待服务器启动

    // 创建客户端socket
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);

    // 连接服务器
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9877);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        close(sockfd);
        return;
    }

    // 发送测试数据
    const char* test_msg = "Hello netlib!";
    ssize_t sent = send(sockfd, test_msg, strlen(test_msg), 0);
    assert(sent == strlen(test_msg));

    // 接收回显
    char buffer[1024]{};
    ssize_t received = recv(sockfd, buffer, sizeof(buffer), 0);
    assert(received == strlen(test_msg));
    assert(strcmp(buffer, test_msg) == 0);

    std::cout << "Test passed!\n";
    close(sockfd);
}

int main() {
    EventLoop loop;
    InetAddress listenAddr(9877);
    
    // 启动服务器
    EchoServer server(&loop, listenAddr);
    server.start();

    // 启动客户端测试线程
    std::thread client_thread(test_client);

    // 运行事件循环
    loop.loop();

    client_thread.join();
    return 0;
}