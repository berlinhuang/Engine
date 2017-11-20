//
// Created by root on 17-11-14.
//

#include <poll.h>
#include <assert.h>
#include "./EventLoop.h"
#include "./Poller.h"
#include "./Channel.h"
#include "./../base/Logging.h"

__thread EventLoop* t_loopInThisThread = 0;// rec eventloop

const int kPollTimeMs = 10000;





EventLoop::EventLoop()
:looping_(false),
 threadId_(CurrentThread::tid()),
 poller_(new Poller(this))
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
    quit_ = false;
    while(!quit_)
    {
        activeChannels_.clear();// typedef std::vector<Channel*> ChannelList;
        // 1.监听并获取待处理的事件，把这些事件放在aciveChannels中
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        // 2.循环处理所有activeChannels_中的事件
        for(ChannelList::iterator it = activeChannels_.begin();it!=activeChannels_.end();++it)
        {
            (*it)->handleEvent();
        }
    }
//    ::poll(NULL, 0, 5*1000);
    LOG_TRACE<<" EventLoop "<< this<< " stop looping";
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}








