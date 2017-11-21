//
// Created by root on 17-11-21.
//

#ifndef ENGINE_TIMERQUEUE_H
#define ENGINE_TIMERQUEUE_H

#include "./../base/Timestamp.h"
//#include "./Timer.h"
//#include "./TimerId.h"
#include "./Channel.h"
#include "Callbacks.h"

#include <set>

class EventLoop;
class Timer;
class TimerId;


class TimerQueue {
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();



    TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

private:
    typedef std::pair<Timestamp, Timer*>Entry;
    typedef std::set<Entry> TimerList;
    TimerList  timers_;


    void handleRead();

    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    void addTimerInLoop(Timer* timer);

    void cancelInLoop(TimerId timerId);

    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    bool callingExpiredTimers_; /* atomic */


    typedef std::pair<Timer*, int64_t> ActiveTimer;//Timer*指针和定时器序列号
    typedef std::set<ActiveTimer> ActiveTimerSet;
    ActiveTimerSet activeTimers_;
    ActiveTimerSet cancelingTimers_;//取消了的定时器的集合
};


#endif //ENGINE_TIMERQUEUE_H
