
#include <sys/timerfd.h>
#include <unistd.h>
#include <cstring>

#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include "Logger.h"

namespace mynetlib
{

int createTimerfd()
{
    /**
     * CLOCK_MONOTONIC：绝对时间
     * TFD_NONBLOCK：非阻塞
     */
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0)
    {
        LOG_FATAL("Failed in timerfd_create\n");
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if(microseconds < 100)
    {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds%Timestamp::kMicroSecondsPerSecond)*1000);
    return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    if(n != sizeof(howmany))
    {
        LOG_ERROR("TimerQueue::handleRead() reads %d bytes instead of 8", (int)n);
    }
}

// 重置timerfd
void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof(newValue));
    memset(&oldValue, 0, sizeof(oldValue));
    // 剩下时间 = 超时时间 - 当前时间
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    // 此函数会唤醒事件循环
    if(ret)
    {
        LOG_ERROR("timerfd_settime error\n");
    }

}

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
     timerfd_(createTimerfd()),
     timerfdChannel_(loop, timerfd_),
     timers_(),
     callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(
        std::bind(&TimerQueue::handleRead, this)
    );
    timerfdChannel_.enableReading();
}
TimerQueue::~TimerQueue()
{
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);

    for(const TimerQueue::Entry& timer : timers_)
    {
        delete timer.second;
    }
}

// TimerQueue提供给loop的添加关闭定时器的接口函数。 必须是线程安全的（往往都在其他线程中被调用）
TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval)
{
    TimerPtr timer(new Timer(std::move(cb), when, interval));
    loop_->runInLoop(
        std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequece());
}

void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(
        std::bind(&TimerQueue::cancelInLoop, this, timerId)
    );
}

// 保证线程安全的操作
void TimerQueue::addTimerInLoop(TimerPtr timer)
{
    // 是否取代了最早的定时触发时间
    bool earliestChanged = insert(timer);

    // 我们需要重新设置timerfd_触发时间
    if(earliestChanged)
    {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if(it != activeTimers_.end())
    {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        delete it->first;
        activeTimers_.erase(it);
    }
    else if(callingExpiredTimers_)
    {
        cancelingTimers_.insert(timer);
    }

}

//当timerfd事件就绪时执行
void TimerQueue::handleRead()
{
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    // 遍历到期的定时器，调用回调函数
    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for(const Entry& it : expired)
    {
        it.second->run();
    }
    callingExpiredTimers_ = false;
    // 重新设置这些定时器
    reset(expired, now);
}

// 返回删除的定时器节点 （std::vector<Entry> expired）
//将所有超时的timer移除
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<TimerPtr>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), end, std::back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for(const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequece());
        size_t n = activeTimers_.erase(timer);
    }
    return expired;
}


void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;
    
    for(const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequece());
        // 重复任务则继续执行
        //如果是runEvery事件，继续添加进TimerList中
        if(it.second->repeat() &&
            cancelingTimers_.find(timer) == cancelingTimers_.end())
        {
            it.second->restart(now);
            insert(it.second);
        }
        else
        {
            delete it.second;
        }
    }

    // 如果重新插入了定时器，需要继续重置timerfd
    if(!timers_.empty())
    {
        nextExpire = timers_.begin()->second->expiration();
    }

    if(nextExpire.valid())
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::insert(TimerPtr timer)
{
    bool earliestChanged = false;
    // 获取超时时间
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first)
    {
        // 说明最早的定时器已经被替换了
        earliestChanged = true;
    }
     // 管理定时器的红黑树插入此新节点
    timers_.insert(Entry(when, timer));
    activeTimers_.insert(ActiveTimer(timer, timer->sequece()));
    return earliestChanged;
}

}