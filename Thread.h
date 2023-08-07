#pragma once

#include "noncopyable.h"
#include "CountDownLatch.h"

#include <functional>
#include <thread>
#include <memory>
#include <unistd.h>
#include <string>
#include <atomic>

namespace mymuduo
{
class Thread : noncopyable
{
public:
    // 绑定函数对象 
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc, const std::string &name = std::string());
    ~Thread();

    void start();
    // 阻塞当前线程，直到 *this 标识的线程执行完毕。
    void join();

    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }

    // 记录产生线程的个数,给线程起名用的
    static int numCreated() { return numCreated_; }
private:
    void setDefaultName();

    bool started_;
    // joined_ == true，表示必须等待线程执行工作函数后才能接下去的工作
    bool joined_;
    // 线程直接开始启动了
    // 我们的需求是控制线程启动的时机
    // 需要定义一个智能指针，自己控制线程对象产生的时机
    // std::thread thead_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    // 记录产生线程的个数,给线程起名用的
    static std::atomic_int numCreated_;

    CountDownLatch latch_;
};

}