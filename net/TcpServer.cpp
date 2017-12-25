//
// Created by root on 17-11-24.
//

#include "TcpServer.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"

#include "./../base/Logging.h"

#include <boost/bind.hpp>





TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg, Option option)
:loop_(CHECK_NOTNULL(loop)),
 ipPort_(listenAddr.toIpPort()),
 name_(nameArg),
 acceptor_(new Acceptor(loop, listenAddr, option==kReusePort)),// socket bind
 threadPool_(new EventLoopThreadPool(loop, name_)),
 connectionCallback_(defaultConnectionCallback),// declare in Callbacks.h and define in TcpConnection.cpp
 messageCallback_(defaultMessageCallback),
 nextConnId_(1)
{
    acceptor_->setNewConnectionCallback( boost::bind(&TcpServer::newConnection,this,_1,_2));  // listen socket  NewConnection
}

TcpServer::~TcpServer()
{
    loop_->assertInLoopThread();
    LOG_TRACE<<"TcpServer::~TcpServer ["<<name_<<"] destructing";
    for(ConnectionMap::iterator it(connections_.begin());
            it!=connections_.end();++it)
    {
        TcpConnectionPtr conn = it->second;
        it->second.reset();
        conn->getLoop()->runInLoop(
                boost::bind(&TcpConnection::connectDestroyed,conn)
        );
        conn.reset();
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}


void TcpServer::start()
{
    if(started_.getAndSet(1) == 0)
    {
        //会判断线程个数   多线程直接启动进入循环      单线程(主控线程)先调用回调不进入循环
        threadPool_->start(threadInitCallback_);
        assert(!acceptor_->listenning());
        //自由方法来说，直接boost::bind(函数名, 参数1，参数2，...)
        //类方法来说，直接boost::bind(&类名::方法名，类实例指针，参数1，参数2）
        loop_->runInLoop(
                boost::bind(&Acceptor::listen,get_pointer(acceptor_)));//scoped_ptr
    }

}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    //1. 从threadpool中选一个eventloop
    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf,sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_+buf;

    LOG_INFO<<"TcpServer::newConnection["<<name_
            <<"] -new connection ["<<connName
            <<"] from "<<peerAddr.toIpPort();
    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    //2. 新建一个TcpConnection结构体管理这个连接
    TcpConnectionPtr conn(
            new TcpConnection(ioLoop,connName, sockfd, localAddr, peerAddr));
    connections_[connName]=conn;
    //3. 设置连接的一些属性
    conn->setConnectionCallback(connectionCallback_);// TcpServer::setConnectionCallback
    conn->setMessageCallback(messageCallback_);//TcpServer::setMessageCallback
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(boost::bind(&TcpServer::removeConnection,this,_1));
    //4. 将连接加入到1中所选的eventloop中运行
    ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished,conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
             << "] - connection " << conn->name();
    size_t n = connections_.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
            boost::bind(&TcpConnection::connectDestroyed, conn));
}