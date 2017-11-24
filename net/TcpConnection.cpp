//
// Created by root on 17-11-24.
//

#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "./../base/Logging.h"
TcpConnection::TcpConnection(EventLoop* loop, const string& nameArg, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr)
:loop_(CHECK_NOTNULL(loop)),
 name_(nameArg),
 socket_(new Socket(sockfd)),
 localAddr_(localAddr),
 peerAddr_(peerAddr)
{


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

}