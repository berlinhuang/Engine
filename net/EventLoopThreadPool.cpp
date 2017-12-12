//
// Created by root on 17-12-1.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <stdio.h>


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const string &nameArg)
:baseLoop_(baseLoop),
 name_(nameArg),
 started_(false),
 numThreads_(0),
 next_(0)
{

}


EventLoopThreadPool::~EventLoopThreadPool() {}


//决定了开启几个EventLoopThread
void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i)
    {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
        EventLoopThread* t = new EventLoopThread(cb, buf);

        //ptr_vector< EventLoopThread >  threads_;
        threads_.push_back(t);//

        //std::vector<EventLoop*> loops_;  EventLoop* EventLoopThread::startLoop()
        loops_.push_back(t->startLoop());// 启动start EventLoopThread线程，在进入事件循环之前，会调用cb
    }
    if (numThreads_ == 0 && cb)
    {
        // 只有一个EventLoop，在这个EventLoop进入事件循环之前，调用cb
        cb(baseLoop_);
    }
}



EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop = baseLoop_;
    // std::vector<EventLoop*> loops_;
    if (!loops_.empty())
    {
        // 如果loops_为空，则loop指向baseLoop_
        // 如果不为空，按照round-robin（RR，轮叫）的调度方式选择一个EventLoop
        loop = loops_[next_];//
        ++next_;
        if (implicit_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;// return  loops_[i]  // 如果只有一个线程这里返回的就是主线程
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    baseLoop_->assertInLoopThread();
    EventLoop* loop = baseLoop_;

    if (!loops_.empty())
    {
        loop = loops_[hashCode % loops_.size()];
    }
    return loop;
}


std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    if(loops_.empty())
    {
        return std::vector<EventLoop*>(1,baseLoop_);//包含了1个值为baseLoop_的元素
    }
    else
    {
        return loops_;
    }
}

