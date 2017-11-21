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
    void removeChannel(Channel* channel);

    void assertInLoopThread(){ ownerLoop_->assertInLoopThread();}
private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    EventLoop* ownerLoop_;

    typedef std::map<int,Channel*> ChannelMap;//fd -> channel*
    ChannelMap channels_;

    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;

};


#endif //ENGINE_POLLER_H
