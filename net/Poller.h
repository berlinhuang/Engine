//
// Created by root on 17-11-15.
//

#ifndef ENGINE_POLLER_H
#define ENGINE_POLLER_H
#include <vector>
#include <map>
#include "./EventLoop.h"
#include "./../base/Timestamp.h"

struct pollfd;

class Channel;

class Poller {
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    ~Poller();

    Timestamp poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);

    void assertInLoopThread(){ ownerLoop_->assertInLoopThread();}
private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int,Channel*> ChannelMap;//fd -> channel*

    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};


#endif //ENGINE_POLLER_H
