//
// Created by root on 17-11-24.
//

#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "./../base/Logging.h"

#include <boost/bind.hpp>


void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    LOG_TRACE << conn->localAddress().toIpPort() << " -> "
              << conn->peerAddress().toIpPort() << " is "
              << (conn->connected() ? "UP" : "DOWN");

}

void defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp)
{
    buf->retrieveAll();
}



TcpConnection::TcpConnection(EventLoop* loop, const string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
:loop_(CHECK_NOTNULL(loop)),
 name_(nameArg),
 state_(kConnecting),
 socket_(new Socket(sockfd)),
 channel_(new Channel(loop,sockfd)),
 localAddr_(localAddr),
 peerAddr_(peerAddr)
{
    channel_->setReadCallback(boost::bind(&TcpConnection::handleRead,this,_1));//messageCallback_
    channel_->setWriteCallback(boost::bind(&TcpConnection::handleWrite,this));//writeCompleteCallback_
    channel_->setCloseCallback(boost::bind(&TcpConnection::handleClose,this));//closeCallback_
    channel_->setErrorCallback(boost::bind(&TcpConnection::handleError,this));
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
              << " fd=" << channel_->fd()
              << " state=" << stateToString();
    assert(state_ == kDisconnected);
}


void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();
    if(state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}


const char* TcpConnection::stateToString() const
{
    switch (state_)
    {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnecting";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}
////////////////////////////////////////

void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if (!channel_->isWriting())
    {
        // we are not writing
        socket_->shutdownWrite();
    }
}

void TcpConnection::handleRead( Timestamp receiveTime )
{
    loop_->assertInLoopThread();
//    char buf[65536];
//    ssize_t  n = ::read(channel_->fd(), buf, sizeof buf);
    int savedErrno = 0;
    ssize_t  n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if(n>0)
    {
//        messageCallback_(shared_from_this(),buf, n);
        messageCallback_(shared_from_this(),&inputBuffer_, receiveTime);
    }
    else if(n == 0)
    {
        handleClose();
    }
    else
    {
        errno = savedErrno;
        LOG_SYSERR<<"TcpConnection::handleRead";
        handleError();
    }

}


void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    LOG_TRACE<<"TcpConnection::handleClose fd="<<channel_->fd()<<" state = "<<stateToString();
    assert(state_== kConnected||state_ == kDisconnecting);
    //leave it to dtor
    setState(kDisconnected);
    channel_->disableAll();
//    closeCallback_(shared_from_this());

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);////默认调用defaultConnectionCallback
    closeCallback_(guardThis);

}

void TcpConnection::handleError()
{
    int err  = sockets::getSocketError(channel_->fd());
    LOG_ERROR<<"TcpConnection::handleError [" << name_
             <<"] - SO_ERROR = "<<err<<" " <<strerror_tl(err);

}


void TcpConnection::handleWrite()
{
    loop_->assertInLoopThread();
    if(channel_->isWriting())
    {
        //write(sockfd, buf, count);
        ssize_t n = sockets::write(channel_->fd(),
                                   outputBuffer_.peek(),
                                   outputBuffer_.readableBytes());
        if(n > 0)
        {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();
                if(writeCompleteCallback_)
                {
                    loop_->queueInLoop((boost::bind(writeCompleteCallback_, shared_from_this())));
                }
                if(state_ == kDisconnecting )
                {
                    shutdownInLoop();
                }
            }
        }
        else
        {
            LOG_SYSERR<<"TcpConnection::handleWrite";
        }
    }
    else
    {
        LOG_TRACE<<"Connection fd = "<<channel_->fd()
                                     <<"is down, not more writing";
    }
}



