//
// Created by root on 17-11-24.
//

#ifndef ENGINE_TCPCONNECTION_H
#define ENGINE_TCPCONNECTION_H

#include "Callbacks.h"
#include "./InetAddress.h"// forward declare seems not working

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//class InetAddress;
class EventLoop;
class Socket;
class Channel;



class TcpConnection:public boost::enable_shared_from_this<TcpConnection>
{

public:
    TcpConnection(EventLoop* loop,
                  const string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr
    );

    ~TcpConnection();


    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }


    void connectEstablished();
    void connectDestroyed();

private:


    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecing };
    StateE state_;  // FIXME: use atomic variable
    void setState(StateE s) { state_ = s; }


    EventLoop* loop_;
    const string name_;

    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;


    const InetAddress localAddr_;
    const InetAddress peerAddr_;


    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};


#endif //ENGINE_TCPCONNECTION_H
