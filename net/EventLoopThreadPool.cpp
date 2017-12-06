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
        threads_.push_back(t);// ptr_vector< EventLoopThread >  threads_;
        loops_.push_back(t->startLoop());//std::vector<EventLoop*> loops_;  EventLoop* EventLoopThread::startLoop()
    }
    if (numThreads_ == 0 && cb)
    {
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
        // round-robin
        loop = loops_[next_]//
        ++next_;
        if (implicit_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;// return  loops_[i]
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

