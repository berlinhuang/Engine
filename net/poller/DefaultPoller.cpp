//
// Created by root on 17-12-6.
//

#include "./../Poller.h"
#include "./PollPoller.h"
#include "./EPollPoller.h"
#include "./../../base/Logging.h"
#include <stdlib.h>



Poller* Poller::newDefaultPoller(EventLoop* loop)
{
//    LOG_INFO<<::getenv("PATH");
    if (::getenv("USE_EPOLL"))
    {
        return new EPollPoller(loop);
    }
    else
    {
        return new PollPoller(loop);
    }
}