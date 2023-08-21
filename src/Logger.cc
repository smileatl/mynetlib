#include "Logger.h"
#include <iostream>
#include "Timestamp.h"

namespace mynetlib
{

void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

// 获取日志唯一的实例对象
Logger& Logger::instance() {
    static Logger logger;
    return logger;
}
// 设置日志级别
void Logger::setLogLevel(int level) {
    logLevel_ = level;
}

// 写日志
void Logger::log(std::string msg)
{
    std::string buf = "";
    switch (logLevel_)
    {
    case INFO:
        // std::cout << "[INFO]";
        buf += "[INFO]";
        break;
    
    case ERROR:
        // std::cout << "[INFO]";
        buf += "[ERROR]";
        break;
    case FATAL:
        // std::cout << "[INFO]";
        buf += "[FATAL]";
        break;
    case DEBUG:
        // std::cout << "[INFO]";
        buf += "[DEBUG]";
        break;
    default:
        break;
    }
    buf += "Time:" + Timestamp::now().toFormattedString(false) + "Content:" + msg;

    g_output(buf.c_str(), buf.size());
    if(logLevel_ == FATAL)
    {
        g_flush();
        exit(0);
    }
    // std::cout << "print time : " << Timestamp::now().toFormattedString(false) <<  msg << std::endl;
}

// // 写日志  [级别信息] time : msg
// void Logger::log(std::string msg) {
//     std::string buf = "";
//     switch (logLevel_) {
//         case INFO:
//             std::cout << "[INFO]";
//             // buf += "[INFO]";
//             break;
//         case ERROR:
//             std::cout << "[ERROR]";
//             // buf += "[ERROR]";
//             break;
//         case FATAL:
//             std::cout << "[FATAL]";
//             // buf += "[FATAL]";
//             break;
//         case DEBUG:
//             std::cout << "[DEBUG]";
//             // buf += "[DEBUG]";
//             break;
//         default:
//             break;
//     }
//     // buf += "Time:" + Timestamp::now().toFormattedString(false) + "Content:" + msg;

//     // g_output(buf.c_str(), buf.size());
//     // if(logLevel_ == FATAL)
//     // {
//     //     g_flush();
//     //     exit(0);
//     // }


//     // 打印时间 : msg
//     std::cout << Timestamp::now().toString() << " : " << msg << std::endl;
// }

void Logger::setOutPut(OutputFunc out)
{
    g_output = out;
}
void Logger::setFlush(FlushFunc flush)
{
    g_flush = flush;
}


}