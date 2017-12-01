//
// Created by root on 17-11-24.
//

#ifndef ENGINE_TCPSERVER_H
#define ENGINE_TCPSERVER_H

#include "Callbacks.h"
#include "./../base/Atomic.h"
#include <map>
#include <boost/scoped_ptr.hpp>

class EventLoop;
class InetAddress;
class Acceptor;

class TcpServer {
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_= cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_  = cb; }

private:

    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    EventLoop* loop_;
    const std::string name_;
    const string ipPort_;
    boost::scoped_ptr<Acceptor> acceptor_;

    AtomicInt32 started_;

    int nextConnId_;

    //typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;// name => connection
    ConnectionMap connections_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;






};


#endif //ENGINE_TCPSERVER_H
