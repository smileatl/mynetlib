#include "EventLoopThread.h"
#include "EventLoop.h"


namespace mynetlib
{
// 底下几个都是默认构造
EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const std::string& name)
    : loop_(nullptr),
      exiting_(false),
      thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      mutex_(),
      cond_(),
      callback_(cb) {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    thread_.start();  //loop开启时，底层的线程开启了，回调函数threadFunc()就开启了

    EventLoop* loop = nullptr;
    {
        
        std::unique_lock<std::mutex> lock(mutex_);
        // 用while防止被其他线程抢走了，确保资源存在
        while (loop_ == nullptr) { // 等待对应的 EventLoop创建
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

// 下面这个方法，是在单独的新线程里面运行的，即thread_.start()内的func()
// 这才是真真正正执行的线程
void EventLoopThread::threadFunc() {
    // 创建了一个独立的eventloop，和上面的线程是一一对应的，one loop per thread
    // 栈上分配
    EventLoop loop;  

    if (callback_) {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();  // EventLoop loop  => Poller.poll
    // 创建了一个独立的eventloop，和上面的线程是一一对应的，one loop per thread 
    // 同样用锁进行了保护
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}



}
