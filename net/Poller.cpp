//
// Created by root on 17-11-15.
//

#include "Poller.h"
#include "./../base/Logging.h"
#include "./Channel.h"
#include <poll.h>

Poller::Poller(EventLoop* loop):ownerLoop_(loop)
{
}


Poller::~Poller()
{
}


Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(),pollfds_.size(),timeoutMs);//typedef std::vector<struct pollfd> PollFdList;
    Timestamp now(Timestamp::now());

    if(numEvents>0)
    {
        LOG_TRACE<<numEvents<<" events happended";
        fillActiveChannels(numEvents, activeChannels);//////////// traverse the
    }
    else if(numEvents==0)
    {
        LOG_TRACE<<"nothing happended";
    }
    else
    {
        LOG_SYSERR<<"Poller::poll()";
    }
    return now;
}


void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    ////typedef std::vector<struct pollfd> PollFdList;       PollFdList pollfds_;
    for(PollFdList::const_iterator pfd = pollfds_.begin();
        pfd!=pollfds_.end()&&numEvents>0;
        ++pfd)
    {
        if(pfd->revents>0)
        {
            --numEvents;
            // typedef std::map<int,Channel*> ChannelMap;//fd -> channel*
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);// return a iterator
            assert(ch!=channels_.end());//断言找到事件
            Channel* channel = ch->second;
            assert(channel->fd() == pfd->fd);
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel *channel)
{
    assertInLoopThread();
    LOG_TRACE<<"fd= "<<channel->fd()<<" events = "<<channel->events();
    if(channel->index()<0)// index()返回在poll的事件数组中的序号，index_在构造函数中的初始值为-1  index < 0说明是一个新的通道
    {
        ////typedef std::map<int,Channel*> ChannelMap;
        assert(channels_.find(channel->fd())==channels_.end());//没找到说明是新的
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events= static_cast<short>(channel->events());
        pfd.revents = 0;
        ////typedef std::vector<struct pollfd> PollFdList;
        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size())-1;
        channel->set_index(idx);
        channels_[pfd.fd] = channel; //channels_ (fd -> channel)
    }
    else// 是已有的通道
    {
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()]==channel);
        int idx = channel->index();
        assert(0<=idx && idx<static_cast<int>(pollfds_.size()));
        struct pollfd& pfd=pollfds_[idx];
        assert(pfd.fd == channel->fd()||pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if(channel->isNoneEvent())//如果channel中已经没有想监听的事件，则将这个channel的fd从epoll中删除
        {
            pfd.fd = -1;
        }
    }
}


//O(logN)
void Poller::removeChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd();
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
    assert(pfd.fd == -1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());
    assert(n == 1);
    (void)n;
    if (implicit_cast<size_t>(idx) == pollfds_.size()-1)
    {
        pollfds_.pop_back();
    }
    else
    {
        int channelAtEnd = pollfds_.back().fd;
        iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
        if (channelAtEnd < 0)
        {
            channelAtEnd = -channelAtEnd-1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}

bool Poller::hasChannel(Channel* channel)
{
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}