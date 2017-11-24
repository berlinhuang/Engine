//
// Created by root on 17-11-24.
//

#ifndef ENGINE_TCPSERVER_H
#define ENGINE_TCPSERVER_H

#include "Callbacks.h"

#include <map>
#include <boost/scoped_ptr.hpp>

class EventLoop;
class InetAddress;
class Acceptor;

class TcpServer {
public:
    TcpServer(EventLoop* loop, const InetAddress& ListenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_= cb;
    }

    void setMessageCallback(const MessageCallback& cb)
    {
        messageCallback_  = cb;
    }

private:

    void newConnection(int sockfd, const InetAddress& peerAddr);


    EventLoop* loop_;
    const std::string name_;

    boost::scoped_ptr<Acceptor> acceptor_;

    bool started_;

    int nextConnId_;

    //typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;// name => connection
    ConnectionMap connections_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;




};


#endif //ENGINE_TCPSERVER_H
