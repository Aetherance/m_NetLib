一个借鉴`muduo`网络库实现的`C++` `Tcp`网络库。用于学习和练习Linux网络编程和C++编程知识。

网络库采用`主从reactor`模式,本网络库依托`基于对象`思想实现，由`EventLoop` `TcpServer` `TcpConnection` 等组件组成。

本网络库仅支持`TCP` `IPv4` 下的网络编程，没有实现其它协议的支持。同时，本网络库也没有实现`http`等额外功能。

网络库没有实现`muduo`实现的所有功能，只实现了以下类。

```
base
├── logger.h
├── noncopyable.h
├── timestamp.cc
└── Timestamp.h
net
├── Acceptor.cc
├── Acceptor.h
├── Buffer.cc
├── Buffer.h
├── Channel.cc
├── Channel.h
├── Epoller.cc
├── Epoller.h
├── EventLoop.cc
├── EventLoop.h
├── EventLoopThread.h
├── EventLoopThreadPool.cc
├── EventLoopThreadPool.h
├── InetAddress.h
├── Poller.cpp
├── Poller.h
├── sigpipe.h
├── Socket.h
├── SocketOps.cc
├── SocketOps.h
├── TcpConnection.cc
├── TcpConnection.h
├── TcpServer.cc
├── TcpServer.h
├── Timer.h
├── TimerId.h
├── TimerQueue.cc
└── TimerQueue.h
```

网络库支持基本的`TCP`多线程服务器编写，采用`非阻塞IO + 事件驱动的IO多路复用`，便于编写较为高效的TCP服务器。