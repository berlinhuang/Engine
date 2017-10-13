//
// Created by root on 17-9-4.
//
#include <string>
#include "Timestamp.h"
#include <inttypes.h>
#include <sys/time.h>
#include <stdio.h>


using std::string;

string Timestamp::toString() const
{
    char buf[32] ={0};
    int64_t  seconds = microSecondsSinceEpoch_/kMicroSecondsPersecond;
    int64_t  microseconds = microSecondsSinceEpoch_%kMicroSecondsPersecond;
    snprintf(buf,sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "",seconds, microseconds);
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds*kMicroSecondsPersecond+tv.tv_usec);
}

string Timestamp::toFormattedString(bool showMicroseconds) const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPersecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    // 这个_r表示是线程安全的函数
    if (showMicroseconds)
    {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPersecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1,
                 tm_time.tm_mday,
                 tm_time.tm_hour,
                 tm_time.tm_min,
                 tm_time.tm_sec,
                 microseconds);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1,
                 tm_time.tm_mday,
                 tm_time.tm_hour,
                 tm_time.tm_min,
                 tm_time.tm_sec);
    }
    return buf;

}