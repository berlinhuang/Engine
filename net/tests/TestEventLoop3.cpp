//
// Created by root on 17-11-20.
//


#include "./../EventLoop.h"
#include "./../Channel.h"
#include <sys/timerfd.h>
//#include <stdio.h>

EventLoop* g_loop;

void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}


/**
 struct timespec {
    time_t tv_sec;                // Seconds
    long   tv_nsec;               // Nanoseconds
};
struct itimerspec {
    struct timespec it_interval;  // Interval for periodic timer
    struct timespec it_value;     // Initial expiration
};
 //it_value和it_interval都为0表示停止定时器。
 */
int main()
{
    EventLoop loop;
    g_loop = &loop;


    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);//用于生成一个定时器对象，返回与之关联的文件描述符
    Channel channel(&loop,timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec  howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();
    ::close(timerfd);

}