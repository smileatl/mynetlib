#pragma once
#include "noncopyable.h"
#include "Socket.h"
#include "Channel.h"

#include <functional>

namespace mymuduo
{

// 前置声明
class EventLoop;
class InetAddress;

class Acceptor:noncopyable{
public:
    using NewConnectionCallback=std::function<void(int sockfd, const InetAddress&)>;

    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb){
        newConnectionCallback_=cb;
    }

    bool listening() const {
        return listenning_;
    }

    void listen();
private:
    void handleRead();

    EventLoop *loop_; // Acceptor用的就是用户定义的那个baseLoop，也称作mainLoop

    // 创建普通成员必须引入头文件了
    // 否则会提示不完整的类型
    // 如果是指针或引用成员，可以只加入类的前置声明，在源文件中进行包含头文件
    Socket acceptSocket_; //它负责监听的就是acceptSocket_上的事件
    Channel acceptChannel_; //将acceptSocket_封装成acceptChannel_

    NewConnectionCallback newConnectionCallback_;   //当loop_里的poller发现acceptSocket_上有事件发生时，向loop_返回acceptChannel_，Acceptor执行此回调
    bool listenning_;
    int idleFd_; //用来解决文件描述符枯竭的情况
};


}