//
// Created by root on 17-11-24.
//

#include "Socket.h"
#include "SocketsOps.h"
#include "InetAddress.h"

Socket::~Socket()
{
    sockets::close(sockfd_);
}


void Socket::bindAddress(const InetAddress& addr)
{
    sockets::bindOrDie(sockfd_, addr.getSockAddr());
}


void Socket::listen() {
    sockets::listenOrDie(sockfd_);
}


int  Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(sockfd_, &addr);
    if(connfd>= 0)
    {
        peeraddr->setSockAddrInet6(addr);
    }
    return connfd;
}



void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, static_cast<socklen_t>(sizeof optval));
    // FIXME CHECK
}