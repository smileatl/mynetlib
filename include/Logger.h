#pragma once

#include <string>
#include <cstring>
#include "noncopyable.h"

namespace mynetlib {
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class FixedBuffer : noncopyable {
public:
    // 初始化了成员变量cur_为data_，表示当前可写入数据的位置。
    FixedBuffer() : cur_(data_) {
        // setCookie(cookieStart);
    }

    ~FixedBuffer() {
        // setCookie(cookieEnd);
    }

    // 向缓冲区中追加数据。如果缓冲区剩余空间足够容纳待写入的数据，则将数据复制到缓冲区，并更新cur_指针。
    void append(const char* /*restrict*/ buf, size_t len) {
        if (static_cast<size_t>(avail()) > len) {
            ::memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    // write to data_ directly
    char* current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() { cur_ = data_; }
    void bzero() { ::bzero(data_, sizeof data_); }

    // for used by GDB
    const char* debugString();
    //   void setCookie(void (*cookie)()) { cookie_ = cookie; }
    // for used by unit test
    std::string toString() const { return std::string(data_, length()); }

private:
    const char* end() const { return data_ + sizeof data_; }
    // Must be outline function for cookies.
    //   static void cookieStart();
    //   static void cookieEnd();

    //   void (*cookie_)();
    char data_[SIZE];
    char* cur_;
};



// 定义宏
// LOG_INFO("%s %d", arg1, arg2)
// 用宏来接受可变参, 定义大宏的时候都会用do while

// 将可变参数__VA_ARGS__根据格式字符串logmsgFormat进行格式化，
// 它的作用是在可变参数为空的情况下，省略前面的逗号。这样可以避免在宏展开时出现语法错误。
#define LOG_INFO(logmsgFormat, ...)                       \
    do {                                                  \
        Logger& logger = Logger::instance();              \
        logger.setLogLevel(INFO);                         \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)

#define LOG_ERROR(logmsgFormat, ...)                      \
    do {                                                  \
        Logger& logger = Logger::instance();              \
        logger.setLogLevel(ERROR);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)

#define LOG_FATAL(logmsgFormat, ...)                      \
    do {                                                  \
        Logger& logger = Logger::instance();              \
        logger.setLogLevel(FATAL);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
        exit(-1);                                         \
    } while (0)

// 调试时IO操作
// 如果自己定义了MUDEBUG
#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat, ...)                      \
    do {                                                  \
        Logger& logger = Logger::instance();              \
        logger.setLogLevel(DEBUG);                        \
        char buf[1024] = {0};                             \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
        logger.log(buf);                                  \
    } while (0)
#else
// 否则就什么也不用输出
#define LOG_DEBUG(logmsgFormat, ...)
#endif

// 定义日志的级别 INFO RERROR FATAL（毁灭性错误） DEBUG（可选开关）
enum LogLevel {
    INFO,   // 普通信息
    ERROR,  // 错误信息
    FATAL,  // core信息
    DEBUG,  // 调试信息
};

// 输出一个日志类，单例就好，不需要进行拷贝构造和赋值
class Logger : noncopyable {
public:
    // 获取日志唯一的实例对象
    // 用引用&，这样调用方法的时候方便，可以用.
    static Logger& instance();
    // 设置日志级别
    void setLogLevel(int level);
    // 写日志
    void log(std::string msg);
    using OutputFunc = void (*)(const char*, int);
    using FlushFunc = void (*)();
    static void setOutPut(OutputFunc);
    static void setFlush(FlushFunc);

private:
    // _logLevel; 和系统的变量不产生冲突
    int logLevel_;
};

}  // namespace mynetlib