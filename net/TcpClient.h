//
// Created by root on 17-12-7.
//

#ifndef ENGINE_TCPCLIENT_H
#define ENGINE_TCPCLIENT_H

#include "TcpConnection.h"
#include "./../base/Mutex.h"

class Connector;

typedef boost::shared_ptr<Connector> ConnectorPtr;

class TcpClient {
public:
    TcpClient(EventLoop* loop,const InetAddress& serverAddr, const string& nameArg);
    ~TcpClient();


    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        MutexLockGuard lock(mutex_);
    }

    EventLoop* getLoop() const
    {
        return loop_;
    }

    const string& name() const
    {
        return name_;
    }

    bool retry() const
    {
        return retry_;
    }

    void enableRetry()
    {
        retry_ = true;
    }

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }

    void setMessageCallback( const MessageCallback& cb)
    {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallbck( const WriteCompleteCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }


private:

    /// Not thread safe, but in loop
    void newConnection(int sockfd);
    /// Not thread safe, but in loop
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;

    bool retry_;
    bool connect_;

    int nextConnId_;

    const string name_;
    mutable MutexLock mutex_;
    TcpConnectionPtr connection_;
};


#endif //ENGINE_TCPCLIENT_H
