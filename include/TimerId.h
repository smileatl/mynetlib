#pragma once

namespace mynetlib
{

class Timer;


// 被设计用来取消Timer的，它的结构很简单，只有一个Timer指针和其序列号。TimerQueue为其友元，可以操作其私有数据
class TimerId
{
public:
    
    friend class TimerQueue;

    TimerId()
        : timer_(nullptr),
          sequence_(0)
    {}

    TimerId(Timer* timer, int64_t seq)
        : timer_(timer),
          sequence_(seq)
    {}

private:
    Timer* timer_;
    int64_t sequence_;

};



}
