#pragma once

#include <atomic>

#include "noncopyable.h"
#include "Callbacks.h"
#include "Timestamp.h"

namespace mynetlib
{
 
class Timer : noncopyable
{
public:
    // 构造函数
    Timer(TimerCallback cb, Timestamp when, double interval)
        : callback_(std::move(cb)),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequece_(++s_numCreated_) // 一次性定时器设置为0
    {}

    // 调用此定时器的回调函数
    void run() const
    {
        callback_();
    }

    // 返回此定时器超时时间
    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequece() const { return sequece_; }

    // 重启定时器(如果是非重复事件则到期时间置为0)
    void restart(Timestamp now);

    static int64_t numCreated() { return s_numCreated_; }

private:
    
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequece_;

    static std::atomic_int64_t s_numCreated_;
};



}

