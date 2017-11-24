//
// Created by root on 17-11-24.
//

#ifndef ENGINE_EVENTLOOPTHREAD_H
#define ENGINE_EVENTLOOPTHREAD_H

#include "../base/Mutex.h"
#include "../base/Thread.h"
#include "../base/Condition.h"

class EventLoop;

class EventLoopThread {

public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;
    EventLoopThread( const ThreadInitCallback cb = ThreadInitCallback(),
                    const string& name = string());

    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;

};


#endif //ENGINE_EVENTLOOPTHREAD_H
