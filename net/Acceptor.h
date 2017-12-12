//
// Created by root on 17-11-24.
//

#ifndef ENGINE_ACCEPTOR_H
#define ENGINE_ACCEPTOR_H

#include <boost/function.hpp>
#include "Channel.h"
#include "Socket.h"

class InetAddress;
class EventLoop;


class Acceptor {
public:
    typedef boost::function< void (int sockfd, const InetAddress&) > NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();
    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop* loop_;

    Socket acceptSocket_;// listening socket
    Channel acceptChannel_;// 观察acceptSocket_ readable event

    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_;

};


#endif //ENGINE_ACCEPTOR_H
