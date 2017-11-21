//
// Created by root on 17-11-14.
//

#include <poll.h>
#include <assert.h>
#include "./EventLoop.h"
#include "./Poller.h"
#include "./Channel.h"
#include "./TimerQueue.h"
#include "./../base/Logging.h"
#include "./../base/Mutex.h"

__thread EventLoop* t_loopInThisThread = 0;// rec eventloop

const int kPollTimeMs = 10000;





EventLoop::EventLoop()
:looping_(false),
 quit_(false),
 eventHandling_(false),
 callingPendingFunctors_(false),
 threadId_(CurrentThread::tid()),
 poller_(new Poller(this)),
 timerQueue_(new TimerQueue(this)),
 currentActiveChannel_(NULL)

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


void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_)
    {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}


void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())
    {
        cb();
    }
//    else
//    {
//        queueInLoop(cb);
//    }
}

//void EventLoop::queueInLoop(const Functor& cb)
//{
//    {
//        MutexLockGuard lock(mutex_);
//        pendingFunctors_.push_back(cb);
//    }
//
//    if (!isInLoopThread() || callingPendingFunctors_)
//    {
//        wakeup();
//    }
//}


TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(),interval));
    return timerQueue_->addTimer(cb,time,interval);
}







