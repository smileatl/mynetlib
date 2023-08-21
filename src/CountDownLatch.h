#pragma once 

#include <mutex>
#include <condition_variable>

#include "noncopyable.h"


// CountDownLatch（倒计时）：是一种常用且易用的同步手段。
namespace mynetlib
{

class CountDownLatch : public noncopyable
{
public:
    explicit CountDownLatch(int count);  //倒数几次
    void wait();
    void countDown();

    int getCount();

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int count_;
};

}

