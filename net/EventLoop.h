//
// Created by root on 17-11-14.
//

#ifndef ENGINE_EVENTLOOP_H
#define ENGINE_EVENTLOOP_H

#include "./../base/CurrentThread.h"
#include <sys/types.h>

class EventLoop {
public:
    EventLoop();
    ~EventLoop();



    void loop();


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

private:
    void abortNotInLoopThread();

    bool looping_;
    const pid_t threadId_;

};


#endif //ENGINE_EVENTLOOP_H
