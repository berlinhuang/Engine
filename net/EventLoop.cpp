//
// Created by root on 17-11-14.
//

#include <poll.h>
#include <assert.h>
#include "./EventLoop.h"
#include "./../base/Logging.h"

__thread EventLoop* t_loopInThisThread = 0;// rec eventloop

EventLoop::EventLoop()
:looping_(false),
 threadId_(CurrentThread::tid())
{

    if(t_loopInThisThread)// make sure that one loop per thread
    {
        LOG_INFO<<"Another EventLoop "<< t_loopInThisThread<<" exist in this thread "<< threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }
}



EventLoop::~EventLoop() {
    assert(!looping_);
}


EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL<< "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " <<  CurrentThread::tid();
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    ::poll(NULL, 0, 5*1000);
    LOG_TRACE<<" EventLoop "<< this<< " stop looping";
    looping_ = false;
}

