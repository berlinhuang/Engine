//
// Created by root on 17-11-22.
//

#include "./../../base/CurrentThread.h"
#include "./../../base/Timestamp.h"
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

int main()
{
    printTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;
        loop.runAfter(1,boost::bind(print,"once1"));
    }



}