//
// Created by root on 17-11-23.
//

#ifndef ENGINE_EPOLLPOLLER_H
#define ENGINE_EPOLLPOLLER_H

#include "./../Poller.h"

#include <vector>


struct epoll_event;

class EPollPoller: public Poller
{
public:
    EPollPoller(EventLoop* loop);
    virtual ~EPollPoller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);

private:
    static const int kInitEventListSize = 16;

    static const char* operationToString(int op);

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

    typedef std::vector<struct epoll_event> EventList;
    EventList events_;

    int epollfd_;

};


#endif //ENGINE_EPOLLPOLLER_H
