//
// Created by root on 17-12-12.
//

#include "../EventLoop.h"
#include "../EventLoopThread.h"

#include <stdio.h>


void runInThread()
{
    printf("runInThread(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());
}

int main()
{
    printf("main(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());

    EventLoopThread loopThread;
    EventLoop* loop = loopThread.startLoop();

    // 异步调用runInThread，即将runInThread添加到loop对象所在IO线程，让该IO线程执行

    // 主线程调用 loop->runInLoop(runInThread);
    // 由于主线程（不是IO线程）调用runInLoop， 故调用queueInLoop()将runInThead添加到队列，然后wakeup()IO线程
    loop->runInLoop(runInThread);


    sleep(1);
    // runAfter内部也调用了runInLoop，所以这里也是异步调用
    // IO线程在doPendingFunctors()中取loop->runAfter() 要唤醒一下，此时只是执行runAfter()
    // 添加了一个2s的定时器,2s超时，timerfd可读，先handleRead()一下然后执行回调函数runInThread()
    loop->runAfter(2, runInThread);
    sleep(3);
    // 那为什么exit main()之后wakeupFd_还会有可读事件呢？
    // 那是因为EventLoopThead栈上对象析构，在析构函数内loop_->quit(),
    // 由于不是在IO线程调用quit()，故也需要唤醒一下，IO线程才能从poll返回，这样再次循环判断while (!quit_) 就能退出IO线程。
    loop->quit();

    printf("exit main().\n");
}