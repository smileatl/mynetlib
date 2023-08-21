#include <mynetlib/Logger.h>
#include <mynetlib/TcpServer.h>
#include <mynetlib/AsyncLogging.h>
#include <mynetlib/Buffer.h>
#include <functional>
#include <string>

using namespace mynetlib;

off_t kRollSize = 500*1000*1000;

AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
  g_asyncLog->append(msg, len);
}

class EchoServer {
public:
    EchoServer(EventLoop* loop,
               const InetAddress& addr,
               const std::string& name)
        : server_(loop, addr, name), loop_(loop) {
        // 注册回调函数
        // 将用户定义的连接事件处理函数注册进TcpServer中，TcpServer发生连接事件时会执行onConnection函数。
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1));

        // 将用户定义的可读事件处理函数注册进TcpServer中，TcpServer发生可读事件时会执行onMessage函数。
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3));

        // 设置合适的loop线程数量  loopthread
        server_.setThreadNum(3);
    }

    void start() { server_.start(); }

private:
    // 连接建立或者断开的回调
    // 用户定义的连接事件处理函数：当服务端接收到新连接建立请求，则打印Connection
    // UP，如果是关闭连接请求，则打印Connection Down
    // conn参数是TcpConnection对象的shared_ptr
    void onConnection(const TcpConnectionPtr& conn) {
        // conn->connected()返回一个bool值，表明目前连接是建立还是断开
        if (conn->connected()) {
            LOG_INFO("Connection UP : %s",
                     conn->peerAddress().toIpPort().c_str());
        } else {
            LOG_INFO("Connection DOWN : %s",
                     conn->peerAddress().toIpPort().c_str());
        }
    }

    // 可读写事件回调
    // conn参数是收到数据的那个TCP连接
    // buf是已经收到的数据，buf的数据会累积，直到用户从中取走（retrieve）数据
    // buf是指针，表明用户代码可以修改（消费）buffer
    // time是收到数据的确切时间，即epoll_wait(2)返回的时间，注意这个时间通常比read(2)发生的时间略早，可以用于正确测量程序的消息处理延迟
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
        // 服务器获取客户端发送的数据
        std::string msg = buf->retrieveAllAsString();
        // 服务器将数据原封不动的发送给客户端
        conn->send(msg);
        // 发送完关闭，删除掉连接
        conn->shutdown();  // 写端   EPOLLHUP =》 closeCallback_
    }

    EventLoop* loop_;
    TcpServer server_;
};

int main(int argc, char** argv) {
    // char name[256] = { '\0' };
    // strncpy(name, argv[0], sizeof name - 1);
    // AsyncLogging log(::basename(name), kRollSize);
    // log.start();
    // g_asyncLog = &log;
    // Logger::setOutPut(asyncOutput);

    // 这个EventLoop就是main
    // EventLoop，即负责循环事件监听处理新用户连接事件的事件循环器。
    EventLoop loop;

    // InetAddress其实是对socket编程中的sockaddr_in进行封装，使其变为更友好简单的接口而已。
    InetAddress addr(6000);

    // 创建了Acceptor对象 non-blocking listenfd create bind
    EchoServer server(&loop, addr, "EchoServer-01");

    // listen  loopthread  listenfd => acceptChannel => mainLoop =>
    server.start();

    loop.loop();  // 启动mainloop的底层Poller

    return 0;
}


