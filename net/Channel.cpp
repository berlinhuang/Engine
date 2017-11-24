//
// Created by root on 17-11-14.
//

#include "Channel.h"
#include "EventLoop.h" // loop_->updateChannel
#include "../base/Logging.h"
#include <poll.h>


const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN|POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    :loop_(loop),
     fd_(fd),
     events_(0),
     revents_(0),
     index_(-1),
     tied_(false),
     addedToLoop_(false)
{}

Channel::~Channel()
{
    assert(!addedToLoop_);
    if(loop_->isInLoopThread())
    {
//        assert(!loop_->hasChannel(this));
    }
}


void Channel::tie(const boost::shared_ptr<void> & obj) {
    tie_ = obj;
    tied_ = true;
}



//Channel::update
//EventLoop::updateChannel()
//Poller::updateChannel()

void Channel::update()
{
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent()
{
    if(revents_&POLLNVAL)
    {
        LOG_WARN<<"Channel::handle_event() POLLNYAL";
    }

    if(revents_&(POLLERR|POLLNVAL))
    {
        if(errorCallback_) errorCallback_();
    }

    if(revents_&(POLLIN|POLLPRI|POLLRDHUP))
    {
        if(readCallback_) readCallback_();
    }
    if(revents_& POLLOUT)
    {
        if(writeCallback_) writeCallback_();
    }
}