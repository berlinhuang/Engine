//
// Created by root on 17-11-23.
//

#include "EPollPoller.h"

#include "./../../base/Logging.h"
\

#include <boost/static_assert.hpp>


#include <sys/epoll.h>
#include <poll.h>


BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
        :Poller(loop),
         epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
         events_(kInitEventListSize)
{
    if(epollfd_<0)
    {
        LOG_SYSFATAL<<"EPollPoller::EPollPoller";
    }
}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}



Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{

}


void EPollPoller::updateChannel(Channel *channel)
{

}

void EPollPoller::removeChannel(Channel *channel)
{

}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{

}

void EPollPoller::update(int operation, Channel *channel)
{

}


const char* EPollPoller::operationToString(int op)
{
    switch (op)
    {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            assert(false && "ERROR op");
            return "Unknown Operation";
    }
}