//
// Created by root on 17-11-14.
//


#include "../EventLoop.h"
#include "./../../base/Thread.h"

void threadFunc()
{
    printf("threadFunc(): pid = %d , tid = %d\n",getpid(), CurrentThread::tid());
    EventLoop loop;
    loop.loop();
}



int main()
{
    printf("main(): pid = %d, tid=%d\n",getpid(), CurrentThread::tid());

    EventLoop loop;

    Thread thread(threadFunc);

    thread.start();

    loop.loop();

    pthread_exit(NULL);
}