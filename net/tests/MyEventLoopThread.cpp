//
// Created by root on 17-11-24.
//
#include "./../../base/Logging.h"
#include "./../EventLoop.h"
#include "./../EventLoopThread.h"
#include <boost/bind.hpp>

void print(EventLoop* p = NULL)
{
    printf("print: pid = %d, tid = %d, loop = %p\n", getpid(), CurrentThread::tid(), p);
}

void quit(EventLoop* p)
{
    print(p);
    p->quit();
}


int main()
{
    Logger::setLogLevel(Logger::TRACE);
    print();
    {
        EventLoopThread thr1; //never start
    }

    {
        EventLoopThread thr2;
        EventLoop* loop = thr2.startLoop();
        loop->runInLoop(boost::bind(print, loop));
        CurrentThread::sleepUsec(500 * 1000);
    }

    {
        EventLoopThread thr3;
        EventLoop* loop = thr3.startLoop();
        loop->runInLoop(boost::bind(quit, loop));
        CurrentThread::sleepUsec(500 * 1000);
    }

}