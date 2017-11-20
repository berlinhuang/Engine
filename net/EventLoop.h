//
// Created by root on 17-11-14.
//

#ifndef ENGINE_EVENTLOOP_H
#define ENGINE_EVENTLOOP_H

#include "./../base/CurrentThread.h"
#include "./../base/Timestamp.h"
#include <sys/types.h>
#include <boost/scoped_ptr.hpp>
#include <vector>

class Poller;
class Channel;


class EventLoop {

public:
    EventLoop();
    ~EventLoop();


    void loop();
    void quit();
    void updateChannel(Channel* channel);

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
    const pid_t threadId_;// current object's thread


    bool quit_;
    boost::scoped_ptr<Poller> poller_;
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels_;
    Timestamp pollReturnTime_;

};


#endif //ENGINE_EVENTLOOP_H
