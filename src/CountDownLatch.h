#pragma once 

#include <mutex>
#include <condition_variable>

#include "noncopyable.h"


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

