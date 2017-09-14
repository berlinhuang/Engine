//
// Created by root on 17-9-12.
//

#include "Logging.h"
#include "CurrentThread.h"

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;


const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}


Logger::LogLevel  initLogLevel()
{
    if(::getenv("LOG_TRACE"))
        return Logger::TRACE;
    else if(::getenv("LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}

Logger::LogLevel  g_logLevel = initLogLevel();

class T
{
public:
    T(const char* str , unsigned len):str_(str),len_(len)
    {
        assert(strlen(str) == len_);
    }
    const char* str_;
    const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
    s.append(v.str_, v.len_);
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
    s.append(v.data_, v.size_);
    return s;
}
const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};




Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile &file, int line)
:time_(Timestamp::now()),stream_(),level_(level),line_(line),basename_(file)
{
    formatTime();
    CurrentThread::tid();
    stream_<< T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    stream_<< T(LogLevelName[level],6);
    if( savedErrno != 0)
    {
        stream_ <<strerror_tl(savedErrno)<<" (errno="<<savedErrno<<")";
    }
}

void Logger::Impl::formatTime()
{

}



void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}


void Logger::Impl::finish()
{
    //self& operator<<(char v)

    stream_<<" - "<< basename_<<":"<<line_<<"\n";
}

Logger::Logger( SourceFile file, int line):impl_(INFO, 0, file, line)
{
}

Logger::Logger( SourceFile file, int line, LogLevel level):impl_(level, 0, file, line)
{
}

Logger::Logger( SourceFile file, int line, LogLevel level, const char* func):impl_(level, 0, file, line)
{
    impl_.stream_<<func<<' ';
}

Logger::Logger( SourceFile file, int line, bool toAbort):impl_(toAbort?FATAL:ERROR, errno, file, line)
{
}


Logger::~Logger()
{
    impl_.finish();

}




















