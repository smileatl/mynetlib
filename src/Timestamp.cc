#include "Timestamp.h"
#include <sys/time.h>
#include <time.h>

namespace mynetlib
{
Timestamp::Timestamp() : microSecondsSinceEpoch_(0) {}
// 构造函数初始化列表的语法
Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch) {}
Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
std::string Timestamp::toString() const {
    char buf[128] = {0};
    tm* tm_time = localtime(&microSecondsSinceEpoch_);
    snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d",
             // tm结构体里的原因，所以要年+1900，月+1
             tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
             tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
    // C形式的字符串
    return buf;
}

// 将时间戳转换为格式化的字符串表示
std::string Timestamp::toFormattedString(bool showMicroseconds) const {
    // 字符数组 buf，并使用大括号初始化 {0} 将其所有元素置为0，以确保结果字符串的末尾被正确终止
    char buf[64] = {0};
    // 微秒数除以一秒中的微秒数（kMicroSecondsPerSecond）得到秒数，然后将其转换为 time_t 类型
    time_t seconds =
        static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    // 使用 gmtime_r() 函数将秒数转换为协调世界时（GMT/UTC）的时间结构体 tm_time。这里使用了线程安全版本的函数 gmtime_r()
    gmtime_r(&seconds, &tm_time);

    // 参数 showMicroseconds 为 true，则需要将微秒数也包含在格式化字符串中。
    if (showMicroseconds) {
        // 通过对微秒数取模得到剩余的微秒数，并将其转换为整数类型
        int microseconds =
            static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
        // 然后使用 snprintf() 函数将格式化的字符串写入到字符数组 buf 中。
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
    } else {
        // 如果参数 showMicroseconds 为 false，则只格式化日期和时间，不包含微秒数。
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    // 最后返回
    return buf;
}

// std::string Timestamp::toFormattedString(bool showMicroseconds) const
// {
//   char buf[64] = {0};
//   time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
//   struct tm tm_time;
//   gmtime_r(&seconds, &tm_time);

//   if (showMicroseconds)
//   {
//     int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
//     snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
//              tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
//              tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
//              microseconds);
//   }
//   else
//   {
//     snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
//              tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
//              tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
//   }
//   return buf;
// }


}

// #include <iostream>
// int main()
// {
//     std::cout << Timestamp::now().toString() << std::endl;
//     return 0;
// }
