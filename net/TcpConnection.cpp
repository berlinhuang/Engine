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
 reading_(true),
 socket_(new Socket(sockfd)),
 channel_(new Channel(loop,sockfd)),
 localAddr_(localAddr),
 peerAddr_(peerAddr),
 highWaterMark_(64*1024*1024)
{
    channel_->setReadCallback(boost::bind(&TcpConnection::handleRead,this,_1)); //messageCallback_  // 通道可读事件到来的时候 回调TcpConnection::handleRead,_1是事件发生时间
    channel_->setWriteCallback(boost::bind(&TcpConnection::handleWrite,this));  //writeCompleteCallback_
    channel_->setCloseCallback(boost::bind(&TcpConnection::handleClose,this));  //closeCallback_
    channel_->setErrorCallback(boost::bind(&TcpConnection::handleError,this));
    LOG_DEBUG << "TcpConnection::ctor[" <<  name_
              << "] at " << this
              << " fd=" << sockfd;
    socket_->setKeepAlive(true);
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


// 不能跨线程调用
void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) //关注POLLOUT事件时处于isWriteing中 不能关闭
    {
        // 这里关闭了写的一半 连接状态改为kDisconnecting 并没有关闭连接 服务器主动调用shutdown 主动断开连接 客户端read为0
        // 服务器端会受到POLLHUP | POLLIN
        // we are not writing
        socket_->shutdownWrite();
    }
}

void TcpConnection::shutdown()
{
    // FIXME: use compare and swap
    if (state_ == kConnected) {
        setState(kDisconnecting);
        // FIXME: shared_from_this()?
        loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop, this));
    }
}


void TcpConnection::forceClose()
{
    // FIXME: use compare and swap
    if (state_ == kConnected || state_ == kDisconnecting)
    {
        setState(kDisconnecting);
        loop_->queueInLoop(boost::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}


void TcpConnection::forceCloseInLoop()
{
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting)
    {
        // as if we received 0 byte in handleRead();
        handleClose();
    }
}


///////////////////////////////////////

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (state_ == kDisconnected)
    {
        LOG_WARN << "disconnected, give up writing";
        return;
    }

    //没有关注可写事件，并且缓冲区数目为0，直接发送
    // if no thing in output queue, try writing directly
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = sockets::write(channel_->fd(), data, len);
        if (nwrote >= 0)
        {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_)
            {
                loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
            }
        }
        else // nwrote < 0
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                LOG_SYSERR << "TcpConnection::sendInLoop";
                if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
                {
                    faultError = true;
                }
            }
        }
    }

    //没有错误，并且还有未写完的数据(说明内核缓冲区满，要将未写完的数据添加到output buffer中)
    assert(remaining <= len);
    if (!faultError && remaining > 0)
    {
        //如果超过高水位标志，逻辑可能有问题，回调highWaterMarkCallback，不过还是要写
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_
            && oldLen < highWaterMark_
            && highWaterMarkCallback_)
        {
            loop_->queueInLoop(boost::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
        }

        //把剩余数据追加到outputbuffer，并注册POLLOUT事件
        outputBuffer_.append(static_cast<const char*>(data)+nwrote, remaining);
        if (!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}


void TcpConnection::sendInLoop(const StringPiece& message)
{
    sendInLoop(message.data(), message.size());
}

void TcpConnection::send(const void* data, int len)
{
    send(StringPiece(static_cast<const char*>(data), len));
}

void TcpConnection::send(const StringPiece& message)
{
    if (state_ == kConnected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            loop_->runInLoop(
                    boost::bind(&TcpConnection::sendInLoop,
                                this,     // FIXME
                                message.as_string()));
            //std::forward<string>(message)));
        }
    }
}

void TcpConnection::send(Buffer* buf)
{
    if (state_ == kConnected)
    {
        if (loop_->isInLoopThread())
        {
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        }
        else
        {
            loop_->runInLoop(
                    boost::bind(&TcpConnection::sendInLoop,
                                this,     // FIXME
                                buf->retrieveAllAsString()));
            //std::forward<string>(message)));
        }
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

// 内核缓冲区有空间了 回调该函数
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



void TcpConnection::setTcpNoDelay(bool on)
{
    socket_->setTcpNoDelay(on);
}

void TcpConnection::startRead()
{
    loop_->runInLoop(boost::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop()
{
    loop_->assertInLoopThread();
    if (!reading_ || !channel_->isReading())
    {
        channel_->enableReading();
        reading_ = true;
    }
}


void TcpConnection::stopRead()
{
    loop_->runInLoop(boost::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop()
{
    loop_->assertInLoopThread();
    if (reading_ || channel_->isReading())
    {
        channel_->disableReading();
        reading_ = false;
    }
}