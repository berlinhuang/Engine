//
// Created by root on 17-11-24.
//

#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOps.h"
#include <boost/bind.hpp>

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr)
:loop_(loop),
 acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),                         //       socket
 acceptChannel_(loop,acceptSocket_.fd()),
 listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);                                                    //        bind
    acceptChannel_.setReadCallback(boost::bind(&Acceptor::handleRead,this));// bind
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    ::close(idleFd_);
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();                                                                   //        listen
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);
    int connfd = acceptSocket_.accept(&peerAddr);                                             //        accept
    if(connfd>0)
    {
        if(newConnectionCallback_)
        {
            newConnectionCallback_(connfd,peerAddr);
        }
        else
        {
            sockets::close(connfd);
        }
    }

}