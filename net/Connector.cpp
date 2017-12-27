//
// Created by root on 17-12-7.
//
#include "Connector.h"

#include "./../base/Logging.h"

#include "EventLoop.h"
#include "SocketsOps.h"
#include "Channel.h"



#include <boost/bind.hpp>

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
:loop_(loop),
 serverAddr_(serverAddr),
 connect_(false),
 state_(kDisconnected),
 retryDelayMs_(kInitRetryDelayMs) //初始化延时
{
    LOG_DEBUG << "ctor[" << this << "]";
}


Connector::~Connector()
{
    LOG_DEBUG << "dtor[" << this << "]";
    assert(!channel_);
}

//////////////////////////////////////////////
void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(boost::bind(&Connector::startInLoop,this));// FIXME: unsafe
}


void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if(connect_) //调用前必须connect_为true，start()函数中会这么做
    {
        connect();
    }
    else
    {
        LOG_DEBUG<<"do not connect";
    }
}


void Connector::restart()
{
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}


void Connector::stopInLoop()
{
    loop_->assertInLoopThread();
    if (state_ == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::stop()
{
    connect_ =false;
    loop_->queueInLoop(boost::bind(&Connector::stopInLoop,this));// FIXME: unsafe
    // FIXME: cancel timer
}


void Connector::connect()
{
    int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());                                   // socket
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());                                        // connect
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
        case 0:             //连接成功
        case EINPROGRESS:   //连接操作正在进行中，但是仍未完成  这边可以用select来实现超时
        case EINTR:
        case EISCONN:       //连接成功
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd); //这几种情况不能重连，
            break;

        default:
            LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            // connectErrorCallback_();
            break;
    }
}


void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    //设置可写回调函数，这时候如果socket没有错误，sockfd就处于可写状态
    channel_->setWriteCallback( boost::bind(&Connector::handleWrite, this)); // FIXME: unsafe
    channel_->setErrorCallback( boost::bind(&Connector::handleError, this)); // FIXME: unsafe
    // channel_->tie(shared_from_this()); is not working,
    // as channel_ is not managed by shared_ptr
    channel_->enableWriting();  //关注可写事件  
}


int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    // Can't reset channel_ here, because we are inside Channel::handleEvent
    loop_->queueInLoop(boost::bind(&Connector::resetChannel, this)); // FIXME: unsafe
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{
    LOG_TRACE << "Connector::handleWrite " << state_;

    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err)
        {
            LOG_WARN << "Connector::handleWrite - SO_ERROR = " << err << " " << strerror_tl(err);
            retry(sockfd);
        }
        else if (sockets::isSelfConnect(sockfd))
        {
            LOG_WARN << "Connector::handleWrite - Self connect";
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
    else
    {
        // what happened?
        assert(state_ == kDisconnected);
    }
}


void Connector::handleError()
{
    LOG_ERROR << "Connector::handleError state=" << state_;
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
        retry(sockfd);
    }
}


//重连函数，采用back-off策略重连，也就是退避策略
//也就是重连时间逐渐延长，0.5s,1s,2s,...一直到30s
void Connector::retry(int sockfd)
{
    sockets::close(sockfd);//先关闭连接
    setState(kDisconnected);
    if (connect_)
    {
        LOG_INFO << "Connector::retry - Retry connecting to " << serverAddr_.toIpPort()
                 << " in " << retryDelayMs_
                 << " milliseconds. ";

        loop_->runAfter(retryDelayMs_/1000.0,  boost::bind(&Connector::startInLoop, shared_from_this()));  //隔一段时间后重连，重新启用startInLoop
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);  //间隔时间2倍增长
    }
    else
    {
        LOG_DEBUG << "do not connect";
    }
}
