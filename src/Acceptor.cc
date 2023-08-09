#include "Acceptor.h"
#include "InetAddress.h"
#include "Logger.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace mynetlib {

static int createNonblocking() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                          IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_FATAL("%s:%s:%d listen socket create err:%d \n", __FILE__,
                  __FUNCTION__, __LINE__, errno);
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop* loop,
                   const InetAddress& listenAddr,
                   bool reuseport)
    : loop_(loop),
      acceptSocket_(createNonblocking()),  // socket
      acceptChannel_(loop, acceptSocket_.fd()),
      listenning_(false),
      idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);  // bind套接字

    // TcpServer::start() Acceptor.listen
    // 有新用户的连接，要执行一个回调（connfd=》channel=》subloop）connfd打包成channel，唤醒一个subloop，来监听后序的读写事件
    // baseLoop => acceptChannel_(listenfd) =>
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    // 不关注fd上任何事件
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    ::close(idleFd_);
    // acceptSocket RAII 自己会析构
}

// 由TcpServer::start()调用
void Acceptor::listen() {
    listenning_ = true;
    // listen
    acceptSocket_.listen();
    // acceptChannel_ => Poller
    acceptChannel_.enableReading();
}

// listenfd有事件发生了，就是有新用户连接了
// 接受新连接，并且以负载均衡的选择方式选择一个subEventLoop，
// 并把这个新连接分发到这个subEventLoop上。
void Acceptor::handleRead() {
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(
                connfd,
                peerAddr);  // 轮询找到subLoop，唤醒，分发当前的新客户端的Channel
        } else {
            ::close(connfd);
        }
    } else {
        LOG_ERROR("%s:%s:%d accept err:%d \n", __FILE__, __FUNCTION__, __LINE__,
                  errno);

        // // 到达上限
        // if (errno == EMFILE) {
        //     LOG_ERROR("%s:%s:%d sockfd reached limit! \n", __FILE__,
        //               __FUNCTION__, __LINE__);
        // }

        // 设一个空的fd占位，当fd资源都满了后，就释放这个空fd，把来的lfd接受再立马关闭，然后再接着占位
        if (errno == EMFILE) {
            ::close(idleFd_);
            ::accept(idleFd_, nullptr, nullptr);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

}  // namespace mynetlib
