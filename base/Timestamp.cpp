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
    int64_t  seconds = microSecondsSinceEpoch_/kMicroSecondsPerSecond;
    int64_t  microseconds = microSecondsSinceEpoch_%kMicroSecondsPerSecond;
    snprintf(buf,sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "",seconds, microseconds);
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds*kMicroSecondsPerSecond+tv.tv_usec);
}

string Timestamp::toFormattedString(bool showMicroseconds) const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    // 这个_r表示是线程安全的函数
    if (showMicroseconds)
    {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
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
/*
 * linux get current time:
 *
 * 1.time_t  time(time_t *tloc)                          time_t             second
 * 2.int    ftime(struct timeb *tp)                      struct timeb        millisecond 已被废弃
 * 3.int    gettimeofday(struct timeval*tv, struct timezone *tz)             microsecond 精度最高，但是它系统调用的开销比 gettimeofday 大
 *                        struct timeval{
 *                              time_t tv_sec;
 *                              suseconds tv_usec;
 *                        }
 * 4.int    clock_gettime(clockid_t clk_id, struct timespect *tp)            nanoseconds
 *                        struct timespec{
 *                              time_t tv_sec;
 *                              long   tv_nsec;
 *                        }
 * 5.gmtime  localtime  timegm   mktime  strftime  struct tm 这些与当前时间无关
 */


/*用于让程序等待一段时间或安排计划任务：
 * 1. sleep
 * 2. alarm
 * 3. usleep
 * sleep / alarm / usleep 有可能用了信号 SIGALRM，在多线程程序中处理信号是个相当麻烦的事情，应当尽量避免
 *
 * 4. nansleep
 * 5. clock_nanosleep
 * 是线程安全的，但是在非阻塞网络编程中，绝对不能用让线程挂起的方式来等待一段时间，程序会失去响应。正确的做法是注册一个时间回调函数
 *
 *
 * 6. getitimer/ setitimer
 * 7. timer_create /timer_settime /timer_gettime /timer_delete
 * 8. timerfd_create /timerfd_gettime /timerfd_settime
 *
 * timerfd_create 把时间变成了一个文件描述符，该“文件”在定时器超时的那一刻变得可读，这样就能很方便地融入到 select/poll 框架中，用统一的方式来处理 IO 事件和超时事件，这也正是 Reactor 模式的长处
 * 传统的 Reactor 利用 select/poll/epoll 的 timeout 来实现定时功能，但 poll 和 epoll 的定时精度只有毫秒，远低于 timerfd_settime 的定时精度
 */
































