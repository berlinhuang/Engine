//
// Created by root on 17-11-14.
//

#ifndef ENGINE_EVENTLOOP_H
#define ENGINE_EVENTLOOP_H

#include "./../base/CurrentThread.h"
#include "./../base/Timestamp.h"
#include "./../base/Mutex.h"
#include "./Callbacks.h"
#include "./TimerId.h"

#include <sys/types.h>
#include <boost/scoped_ptr.hpp>
#include <vector>

class Poller;
class Channel;
class TimerQueue;

class EventLoop {

public:
    typedef boost::function<void()> Functor;

    EventLoop();
    ~EventLoop();


    void loop();
    void quit();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const
    {
        return threadId_ == CurrentThread::tid();
    }


    static EventLoop* getEventLoopOfCurrentThread();

    void runInLoop(const Functor& cb);

    void queueInLoop(const Functor& cb);

    TimerId runAt(const Timestamp& time, const TimerCallback& cb);

    TimerId runAfter(double delay, const TimerCallback& cb);

    TimerId runEvery(double interval, const TimerCallback& cb);

    void cancel(TimerId timerId);

private:
    void abortNotInLoopThread();

    bool looping_;
    const pid_t threadId_;// current object's thread


    bool quit_;
    boost::scoped_ptr<Poller> poller_;
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels_;
    Timestamp pollReturnTime_;

    boost::scoped_ptr<TimerQueue> timerQueue_;

    bool eventHandling_; /* atomic */
    bool callingPendingFunctors_; /* atomic */

    Channel* currentActiveChannel_;

    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_; // @GuardedBy mutex_

};


#endif //ENGINE_EVENTLOOP_H
