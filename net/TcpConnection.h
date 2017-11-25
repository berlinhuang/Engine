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


    EventLoop* getLoop() const { return loop_; }
    const string& name() const { return name_; }


    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    // when accept a new connection on TcpServer::newConnection
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    void connectEstablished();
    void connectDestroyed();

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();


    const char* stateToString() const;

private:


    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
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
    CloseCallback closeCallback_;
};


#endif //ENGINE_TCPCONNECTION_H
