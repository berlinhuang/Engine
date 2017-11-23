//
// Created by root on 17-11-21.
//

#include "./../base/Logging.h"
#include "TimerQueue.h"
#include "./EventLoop.h"
#include "./Timer.h"
#include <boost/bind.hpp>
#include <sys/timerfd.h>



int createTimerfd()
{//生成一个定时器对象
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,//绝对时间为准，获取的时间为系统重启到现在的时间
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_SYSFATAL << "Failed in timerfd_create";
    }
    return timerfd;
}


TimerQueue::TimerQueue(EventLoop* loop)
        : loop_(loop),
          timerfd_(createTimerfd()),
          timerfdChannel_(loop, timerfd_),
          timers_(),
          callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(boost::bind(&TimerQueue::handleRead, this));//channel::handleRead() will callback TimeQueue::handleRead()
    // we are always reading the timerfd, we disarm it with timerfd_settime.
    timerfdChannel_.enableReading();
}


TimerQueue::~TimerQueue()
{
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
    // do not remove channel, since we're in EventLoop::dtor();
    for (TimerList::iterator it = timers_.begin();
         it != timers_.end(); ++it)
    {
        delete it->second;
    }
}

bool TimerQueue::insert(Timer* timer) //若当前插入的定时器比队列中的定时器都早则返回真
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first)//operator <( lhs, rhs)
    {
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result = timers_.insert(Entry(when,timer));
        assert(result.second);
        (void)result;
    }

    {
        std::pair<ActiveTimerSet::iterator, bool> result = activeTimers_.insert(ActiveTimer(timer,timer->sequence()));
        assert(result.second);
        (void)result;
    }
    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;


}

struct timespec howMuchTimeFromNow(Timestamp when)//现在距离超时时间when还有多久
{
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100)
    {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPersecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPersecond) * 1000);
    return ts;
}

// 设置新的超时时间为newValue
void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_SYSERR << "timerfd_settime()";
    }

}

void TimerQueue::addTimerInLoop(Timer *timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);//将timer 插入TimerQueue的timers_ 和activeTimers_ 中
    if(earliestChanged)
    {
        resetTimerfd(timerfd_, timer->expiration()); //更新timefd_
    }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if (it != activeTimers_.end())
    {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1); (void)n;
        delete it->first; // FIXME: no delete please
        activeTimers_.erase(it);
    }
    else if (callingExpiredTimers_)
    {
        cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
}


// 将timer 插入TimerQueue的timers_ 和activeTimers_ 中
// 该函数可以跨线程调用
TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(cb, when, interval);
    loop_ ->runInLoop(boost::bind(&TimerQueue::addTimerInLoop,this,timer));
    return TimerId(timer, timer->sequence());
}


void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(boost::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if (n != sizeof howmany)
    {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}


void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    // safe to callback outside critical section
    for (std::vector<Entry>::iterator it = expired.begin();
         it != expired.end(); ++it)
    {
        it->second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

//已经执行完超时回调的定时器是否需要重置定时
void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;

    for (std::vector<Entry>::const_iterator it = expired.begin();
         it != expired.end(); ++it)
    {
        ActiveTimer timer(it->second, it->second->sequence());
        if (it->second->repeat()
            && cancelingTimers_.find(timer) == cancelingTimers_.end())
        {
            it->second->restart(now);
            insert(it->second);
        }
        else
        {
            // FIXME move to a free list
            delete it->second; // FIXME: no delete please
        }
    }

    if (!timers_.empty())
    {
        nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.valid())
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    assert(timers_.size()==activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    //返回比参数小的下界，即返回第一个当前未超时的定时器(可能没有这样的定时器)
    TimerList::iterator end = timers_.lower_bound(sentry);
    assert(end == timers_.end()|| now < end->first);

    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(),end);

    for(std::vector<Entry>::iterator it = expired.begin();
            it!=expired.end(); ++it)
    {
        ActiveTimer timer(it->second, it->second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1);
        (void)n;
    }
    assert(timers_.size()==activeTimers_.size());
    return expired;//返回已经超时的那部分定时器
}