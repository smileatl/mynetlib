#pragma once

#include <iostream>
#include <string>

namespace mynetlib
{
// 时间类
// 只写了需要用的方法
class Timestamp {
public:
    Timestamp();
    // 必须建立对象，显式；不允许隐式转换
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    static Timestamp now();
    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;

    //返回当前时间戳的微妙
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

    void swap(Timestamp& that) {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};

// 如果是重复定时任务就会对此时间戳进行增加。
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    // 将延时的秒数转换为微妙
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    // 返回新增时后的时间戳
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}