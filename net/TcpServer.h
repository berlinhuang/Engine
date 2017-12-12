//
// Created by root on 17-11-24.
//

#ifndef ENGINE_TCPSERVER_H
#define ENGINE_TCPSERVER_H

#include "Callbacks.h"
#include "./../base/Atomic.h"
#include <map>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

class EventLoop;
class InetAddress;
class Acceptor;
class EventLoopThreadPool;

class TcpServer {
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;
    enum Option
    {
        kNoReusePort,
        kReusePort,
    };
    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg, Option option = kNoReusePort);
    ~TcpServer();

    void start();
    void setThreadNum(int numThreads);

    const string& ipPort() const { return ipPort_; }
    const string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_= cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_  = cb; }

    void setWriteCompleteCallback( const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb;  }
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

    WriteCompleteCallback writeCompleteCallback_;

    ThreadInitCallback threadInitCallback_;

    boost::shared_ptr<EventLoopThreadPool> threadPool_;
};


#endif //ENGINE_TCPSERVER_H
