#pragma once

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "noncopyable.h"
#include "Callbacks.h"
#include "Channel.h"
#include "Timestamp.h"

namespace mynetlib
{

class Timer;
class TimerId;

class TimerQueue : public noncopyable
{

public:


    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    // TimerQueue提供给loop的添加关闭定时器的接口函数。 必须是线程安全的（往往都在其他线程中被调用）
    // 插入定时器（回调函数，到期时间，是否重复）
    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
    void cancel(TimerId timerId);

private:
    using TimerPtr = Timer*;
    using Entry = std::pair<Timestamp, TimerPtr>;
    using TimerList = std::set<Entry>;
    using ActiveTimer = std::pair<TimerPtr, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

    // 在本loop中添加定时器
    // 保证线程安全的操作
    void addTimerInLoop(TimerPtr timer);
    void cancelInLoop(TimerId timerId);

    // 定时器读事件触发的函数
    //当timerfd事件就绪时执行
    void handleRead();

    // 移除所有已到期的定时器
    // 1.获取到期的定时器
    // 2.重置这些定时器（销毁或者重复定时任务）
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    // 插入定时器的内部方法
    bool insert(TimerPtr timer);

    EventLoop* loop_; // 所属的EventLoop
    const int timerfd_; // timerfd是Linux提供的定时器接口
    Channel timerfdChannel_;  // 封装timerfd_文件描述符
    // 存储了Timer， 使用set容器（红黑树），按照Expiration时间进行排序
    TimerList timers_;

    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_; // 正在获取超时定时器标志
    ActiveTimerSet cancelingTimers_;

};

}