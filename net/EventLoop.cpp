//
// Created by root on 17-11-14.
//

#include <poll.h>
#include <assert.h>
#include <sys/eventfd.h>


#include "./EventLoop.h"
#include "./Poller.h"
#include "./Channel.h"
#include "./TimerQueue.h"
#include "./SocketsOps.h"
#include "./../base/Logging.h"
#include "./../base/Mutex.h"

__thread EventLoop* t_loopInThisThread = 0;// rec eventloop

const int kPollTimeMs = 10000; // ns=>ns return  -1=>block  0=>nonblock


int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }
    return evtfd;
}


EventLoop::EventLoop()
:looping_(false),
 quit_(false),
 eventHandling_(false),
 callingPendingFunctors_(false),
 threadId_(CurrentThread::tid()),
 poller_(Poller::newDefaultPoller(this)),
 timerQueue_(new TimerQueue(this)),
 currentActiveChannel_(NULL),
 wakeupFd_(createEventfd())
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


//唤醒EventLoop
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);//随便写点数据进去就唤醒了
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
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
        eventHandling_ = true;
        for(ChannelList::iterator it = activeChannels_.begin();it!=activeChannels_.end();++it)
        {
//            (*it)->handleEvent();
            currentActiveChannel_ = *it;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = NULL;
        eventHandling_ = false;

        // 3.处理完了IO事件之后，处理等待在队列中的任务
        doPendingFunctors();
    }
//    ::poll(NULL, 0, 5*1000);
    LOG_TRACE<<" EventLoop "<< this<< " stop looping";
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
    {
        wakeup();
    }
}


// 1. 不是简单的在临界区内依次调用functor，而是把回调列表swap到functors中，这一方面减小了
//临界区的长度，意味着不会阻塞其他线程的queueInLoop()，另一方面也避免了死锁(因为Functor可能再次调用queueInLoop)

// 2. 由于doPendingFunctors()调用的Functor可能再次调用queueInLoop(cb)，这是queueInLoop()就必须wakeup(),否则新增的cb可能就不能及时调用了

// 3. muduo没有反复执行doPendingFunctors()直到pendingFunctors为空，这是有意的，否则I/O线程可能陷入死循环，无法处理I/O事件

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < functors.size(); ++i)
    {
        functors[i]();
    }
    callingPendingFunctors_ = false;
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

bool EventLoop::hasChannel(Channel* channel)
{
    assert(channel->ownerLoop()==this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}


void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread())// 同步IO
    {
        cb();// 如果是当前线程 调用同步cb
    }
    else// 异步用来计算
    {
        queueInLoop(cb);// 如果是其他线程调用 则异步地将cb添加到队列中
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        MutexLockGuard lock(mutex_);//其他线程调用必须获得锁
        pendingFunctors_.push_back(cb);
    }
    //如果当前调用queueInLoop调用者不是I/O线程，那么唤醒该I/O线程，以便I/O线程及时处理。
    //或者调用的线程是当前I/O线程，并且此时调用pendingfunctor，需要唤醒
    //只有当前I/O线程的事件回调中调用queueInLoop才不需要唤醒
    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();//加入要执行的事件到队列中后，我们当然希望I/O线程立刻执行该任务，所以立刻调用wakeup函数，唤醒I/O线程
    }
}


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


void EventLoop::cancel(TimerId timerId)
{
    return timerQueue_->cancel(timerId);
}






