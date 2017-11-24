//
// Created by root on 17-11-24.
//


#include "./../EventLoop.h"
#include "./../../base/Thread.h"
#include <stdio.h>
EventLoop* g_loop;

void print()
{
    printf("print(): pid = %d , tid = %d\n",getpid(), CurrentThread::tid());
}


void threadFunc()//
{
    printf("threadFunc(): pid = %d , tid = %d\n",getpid(), CurrentThread::tid());
    g_loop->runAfter(1.0,print);
}

////子线程调用g->runAfter 主线程循环poll超时返回去执行 doPendingFunctors
int main()
{
    printf("main(): pid = %d , tid = %d\n",getpid(), CurrentThread::tid());
    EventLoop loop;
    g_loop = &loop;
    Thread t(threadFunc);
    t.start();//thread 1
    loop.loop();// main thread                    EventLoop::runInLoop()  will call queueInLoop(cb);   so  addTimer() is thread safe
}