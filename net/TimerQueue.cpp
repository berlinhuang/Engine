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

bool TimerQueue::insert(Timer* timer)
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
    bool earliestChanged = insert(timer);
    if(earliestChanged)
    {
        resetTimerfd(timerfd_, timer->expiration());
    }
}


TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(cb, when, interval);
    loop_ ->runInLoop(boost::bind(&TimerQueue::addTimerInLoop,this,timer));
    return TimerId(timer, timer->sequence());
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


}