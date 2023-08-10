#pragma once

#include <atomic>
#include <functional>
#include <any>
#include <memory>
#include <mutex>
#include <vector>

#include "CurrentThread.h"
#include "Timestamp.h"
#include "noncopyable.h"
#include "TimerId.h"
#include "Callbacks.h"

namespace mynetlib
{

class Channel;
class Poller;
class TimerQueue;

// 时间循环类  主要包含了两个大模块 Channel   Poller（epoll的抽象）
class EventLoop : noncopyable {
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    // 开启事件循环
    void loop();
    // 退出事件循环
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    // 在当前loop中执行cb
    void runInLoop(Functor cb);
    // 把cb放入队列中，唤醒loop所在的线程，执行cb
    void queueInLoop(Functor cb);

    // 用来唤醒loop所在的线程的
    void wakeup();

    // EventLoop的方法 =》 Poller的方法
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // 判断EventLoop对象是否在自己的线程里面
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    bool eventHandling() const { return eventHandling_; }

    void setContext(const std::any& context)
    { context_ = context; }

    const std::any& getContext() const
    { return context_; }

    std::any* getMutableContext()
    { return &context_; }

    /******timers********/
    TimerId runAt(Timestamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
    void cancel(TimerId timerId);

private:
    void handleRead();  // wake up  通过给wakeupfd上写入数据，唤醒ioLoop
    void doPendingFunctors();  // 执行回调

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_;  // 原子操作，通过CAS是实现的
    std::atomic_bool quit_;     // 标识退出loop循环
    std::atomic_bool eventHandling_;    // 表示是否在处理事件

    // 调用某个loop对象的线程未必是进行loop操作的线程
    // 记录当前EventLoop进行loop操作所在线程的tid，确保Channel回调在其对应的evnetloop中执行 
    const pid_t threadId_; 

    Timestamp pollReturnTime_;  // poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    // 用了eventfd
    int wakeupFd_;  // 主要作用，当mainLoop获取一个新用户的channel，通过轮询算法选择一个subloop，通过该成员唤醒subloop处理channel
    // 别的线程唤醒本loop线程使用的Channel
    std::unique_ptr<Channel> wakeupChannel_;

    std::any context_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    std::atomic_bool
        callingPendingFunctors_;  // 标识当前loop是否有需要执行的回调操作
    std::vector<Functor> pendingFunctors_;  // 存储loop需要执行的所有的回调操作
    std::mutex mutex_;  // 互斥锁，用来保护上面vector容器的线程安全操作（保护 pendingFunctors_ 线程安全操作）
};


}