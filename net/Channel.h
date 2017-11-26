//
// Created by root on 17-11-14.
//

#ifndef ENGINE_CHANNEL_H
#define ENGINE_CHANNEL_H


#include "../base/Timestamp.h"
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>


class EventLoop;

//Channel的成员函数都只能在IO线程调用，因此更新数据成员不用加锁
// 记录了描述符fd的注册事件和就绪事件以及就绪事件的回调函数；
// 只属于一个Eventloop，每个channel对象自始至终只负责一个文件描述符fd的IO事件的分发
// 但它并不拥有这个fd，把不同的IO事件分发为不同的回调，如：ReadCallback和WriteCallback
class Channel {
public:
    typedef boost::function<void()> EventCallback;
    typedef boost::function<void(Timestamp)> ReadEventCallback;

    Channel( EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);
    void handleEventWithGuard(Timestamp receiveTime);

    //TcpConnection::TcpConnection will call
    void setReadCallback(const ReadEventCallback& cb){ readCallback_ = cb;}
    void setWriteCallback(const EventCallback& cb){ writeCallback_ = cb;}
    void setErrorCallback(const EventCallback& cb){ errorCallback_ = cb;}
    void setCloseCallback(const EventCallback& cb){ closeCallback_ = cb;}

    void tie(const boost::shared_ptr<void>&);


    int fd() const { return  fd_;}
    int events() const {return events_;}
    void set_revents(int revt){ revents_ = revt;}
    bool isNoneEvent()const { return events_ == kNoneEvent;}

    void enableReading(){ events_ |= kReadEvent; update();}// 将相应的Channel事件处理器加入到eventpool中
    void disableAll() { events_ = kNoneEvent; update(); }

    int index(){ return index_;}
    void set_index(int idx){ index_ = idx;}

    //for debug
    string reventsToString() const;
    string eventsToString() const;



    EventLoop* ownerLoop(){ return loop_;}
    void remove();

private:

    static string eventsToString(int fd, int ev);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    void update();

    EventLoop* loop_;
    const int fd_;


    boost::weak_ptr<void> tie_;
    bool tied_;
    bool       logHup_;


    int events_;
    int revents_;
    int index_;

    bool addedToLoop_;
    bool eventHandling_;
    //typedef boost::function<void(Timestamp)> ReadEventCallback;
    ReadEventCallback readCallback_;
    // typedef boost::function<void()> EventCallback;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};


#endif //ENGINE_CHANNEL_H
