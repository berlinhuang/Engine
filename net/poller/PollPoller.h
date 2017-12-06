//
// Created by root on 17-11-23.
//

#ifndef ENGINE_POLLPOLLER_H
#define ENGINE_POLLPOLLER_H

#include "../Poller.h"

struct pollfd;

class PollPoller: public Poller
{
public:
    PollPoller(EventLoop* loop);
    virtual  ~PollPoller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);


private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;

};


#endif //ENGINE_POLLPOLLER_H
