//
// Created by root on 17-11-15.
//

#ifndef ENGINE_POLLER_H
#define ENGINE_POLLER_H
#include <vector>
#include <map>
#include "./EventLoop.h"
#include "./../base/Timestamp.h"

//struct pollfd;

class Channel;

class Poller {
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    virtual void updateChannel(Channel* channel) = 0;

    virtual void removeChannel(Channel* channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);

    void assertInLoopThread() const { ownerLoop_->assertInLoopThread();}

protected:
    typedef std::map<int,Channel*> ChannelMap;//fd -> channel*
    ChannelMap channels_;
private:
    EventLoop* ownerLoop_;
};


#endif //ENGINE_POLLER_H
