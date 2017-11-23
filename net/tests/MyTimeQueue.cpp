//
// Created by root on 17-11-22.
//

#include "./../../base/CurrentThread.h"
#include "./../../base/Timestamp.h"
#include "./../../base/Logging.h"
#include "./../EventLoop.h"
#include <unistd.h>
#include <stdio.h>


#include <boost/bind.hpp>

EventLoop* g_loop;

int cnt = 0;

void printTid()
{
    printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char* msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20)
    {
        g_loop->quit();
    }
}

void cancel(TimerId timer)
{
    g_loop->cancel(timer);
    printf("cancelled at %s\n", Timestamp::now().toString().c_str());
}


int main()
{
    Logger::setLogLevel(Logger::TRACE);
    printTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;
        loop.runAfter(1,boost::bind(print,"once1"));
        loop.runAfter(1.5, boost::bind(print, "once1.5"));
        loop.runAfter(2.5, boost::bind(print, "once2.5"));
        loop.runAfter(3.5, boost::bind(print, "once3.5"));
        TimerId t45 = loop.runAfter(4.5, boost::bind(print, "once4.5"));
        loop.runAfter(4.2, boost::bind(cancel, t45));
        loop.runEvery(2, boost::bind(print, "every2"));
        loop.loop();
    }



}