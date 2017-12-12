//
// Created by root on 17-11-24.
//

#include "EventLoopThread.h"
#include "EventLoop.h"
#include <boost/bind.hpp>

EventLoopThread::EventLoopThread(const ThreadInitCallback cb, const string &name)
:loop_(NULL),
 exiting_(false),
 thread_(boost::bind(&EventLoopThread::threadFunc,this),name),
 mutex_(),
 cond_(mutex_),
 callback_(cb)
{

}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if(loop_!=NULL)
    {
        loop_->quit();
        thread_.join();
    }
}


EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while (loop_ == NULL)
        {
            cond_.wait();//等待创建好当前IO线程(完成EventLoop对象) 才返回Loop_通知主线程创建完成
        }
    }

    return loop_;
}


// 线程函数
void EventLoopThread::threadFunc()
{
    EventLoop loop;//创建EventLoop对象。注意，在栈上
    if(callback_)
    {
        callback_(&loop);
    }

    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop; //
        cond_.notify();//通知startLoop
    }

    loop.loop();//会在这里循环，直到EventLoopThread析构。此后不再使用loop_访问EventLoop了
    loop_ = NULL;
}