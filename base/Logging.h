//
// Created by root on 17-9-12.
//

/*
 * Logger
 *  LogLevel
 *  SourceFile
 *  Impl
 *
 */

#ifndef ENGINE_LOGGING_H
#define ENGINE_LOGGING_H
#include "LogStream.h"
#include "Timestamp.h"

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class SourceFile
    {
    public:
        template <int N>
        inline SourceFile(const char (&arr)[N]):data_(arr), size_(N-1)
        {
            const char* slash = strrchr(data_, '/');//找一个字符'/'在另一个字符串data_中末次出现的位置
            if(slash)
            {
                data_ = slash + 1;//data_ is a pointer which is at begin of file name
                size_ -= static_cast<int>(data_- arr);
            }
        }

        explicit SourceFile( const char* filename ): data_(filename)
        {
            const char* slash =  strrchr(filename, '/');
            if(slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream(){ return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel( LogLevel level );

    typedef void (*OutputFunc)( const char* msg, int len);
    typedef void (*FlushFunc)();

    static void setOutPut(OutputFunc);
    static void setFlush(FlushFunc);
//    static void setTimeZone(const TimeZone& tz);


private:
    class Impl //实际上上logger类内部的一个嵌套类，封装了Logger的缓冲区stream_
    {
    public:
        typedef Logger::LogLevel LogLevel;
        //级别错误文件行
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();


        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    Impl impl_;

};


extern Logger::LogLevel g_logLevel;
inline Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

#define LOG_TRACE if(Logger::logLevel()<= Logger::TRACE) \
    Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()

#define LOG_DEBUG if(Logger::logLevel()<= Logger::DEBUG) \
    Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()

#define LOG_INFO if(Logger::logLevel()<= Logger::INFO) \
    Logger(__FILE__, __LINE__).stream()

const char* strerror_tl(int savedErrno);



#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__,__LINE__, true).stream()






#endif //ENGINE_LOGGING_H
