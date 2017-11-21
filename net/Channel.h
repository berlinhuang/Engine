//
// Created by root on 17-11-14.
//

#ifndef ENGINE_CHANNEL_H
#define ENGINE_CHANNEL_H

#include <boost/function.hpp>

class EventLoop;

//Channel的成员函数都只能在IO线程调用，因此更新数据成员不用加锁

class Channel {
public:
    typedef boost::function<void()> EventCallback;

    Channel( EventLoop* loop, int fd);

    void handleEvent();

    void setReadCallback(const EventCallback& cb){ readCallback_ = cb;}
    void setWriteCallback(const EventCallback& cb){ writeCallback_ = cb;}
    void setErrorCallback(const EventCallback& cb){ errorCallback_ = cb;}

    int fd() const { return  fd_;}
    int events() const {return events_;}
    void set_revents(int revt){ revents_ = revt;}
    bool isNoneEvent()const { return events_ == kNoneEvent;}

    void enableReading(){ events_ |= kReadEvent; update();}// 将相应的Channel事件处理器加入到eventpool中

    int index(){ return index_;}
    void set_index(int idx){ index_ = idx;}
    EventLoop* ownerLoop(){ return loop_;}


private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    void update();

    EventLoop* loop_;
    const int fd_;

    int events_;
    int revents_;
    int index_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;

};


#endif //ENGINE_CHANNEL_H
