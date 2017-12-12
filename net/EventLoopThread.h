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
    EventLoop* startLoop();// 启动线程，该线程就成为了IO线程

private:
    void threadFunc();

    EventLoop* loop_;  // loop_指针指向一个EventLoop对象
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;// 回调函数在EventLoop::loop事件循环之前被调用

};


#endif //ENGINE_EVENTLOOPTHREAD_H
