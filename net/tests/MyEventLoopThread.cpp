//
// Created by root on 17-11-24.
//
#include "./../../base/Logging.h"
#include "./../EventLoop.h"
#include "./../EventLoopThread.h"
#include <boost/bind.hpp>

void print(EventLoop* p = NULL)
{
    printf("print: pid = %d, tid = %d, loop = %p, time=%s\n", getpid(), CurrentThread::tid(), p, Timestamp::now().toString().c_str());
}

void quit(EventLoop* p)
{
    print(p);
    p->quit();
}


int main()
{
//    Logger::setLogLevel(Logger::TRACE);
    print();
    {
        EventLoopThread thr1; //never start
    }

    printf("time = %s\n",Timestamp::now().toString().c_str());

    //dtor calls quit
    {
        EventLoopThread thr2;// thread_(boost::bind(&EventLoopThread::threadFunc,this),name)
        EventLoop* loop = thr2.startLoop(); //return loop
        loop->runInLoop(boost::bind(print, loop)); // main thread call loop->runInLoop()
        CurrentThread::sleepUsec(500 * 1000);
    }

    {
        EventLoopThread thr3;
        EventLoop* loop = thr3.startLoop();
        loop->runInLoop(boost::bind(quit, loop)); //
        CurrentThread::sleepUsec(500 * 1000);
    }

}