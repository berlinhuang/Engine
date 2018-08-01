//
// Created by root on 18-8-1.
//

#include "Socket.h"
#include "InetAddress.h"


#include <assert.h>
#include <sys/socket.h> //socket()
#include <netinet/in.h> //IPPROTO_TCP
#include <unistd.h>
#include <netinet/tcp.h>

namespace
{
    typedef struct sockaddr SA;
    const SA* sockaddr_cast(const struct sockaddr_in* addr)
    {
        return static_cast<const SA*>(implicit_cast<const void*>(addr));
    }
    SA* sockaddr_cast(struct sockaddr_in* addr )
    {
        return static_cast<SA*>(implicit_cast<void*>(addr));
    }
}


Socket::Socket(int sockfd):sockfd_(sockfd)
{
    assert(sockfd_>=0);
}

Socket::~Socket()
{
    if(sockfd_>=0)
    {
        int ret = ::close(sockfd_);
        assert(ret==0);
        (void)ret;
    }
}

void Socket::setReuseAddr(bool on)
{
    int optval = on?1:0;
    if(::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval)<0)
    {
        perror("SOcket::setReuseAddr");
    }
}


void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                     &optval, static_cast<socklen_t>(sizeof optval)) < 0)
    {
        perror("Socket::setTcpNoDelay");
    }
}

void Socket::bindOrDie(const InetAddress &addr)
{
    const struct sockaddr_in& saddr = addr.getSockAddrInet();
    int ret = ::bind(sockfd_,sockaddr_cast(&saddr),sizeof saddr);
    if(ret)
    {
        perror("Socket::bindOrDie");
        abort();
    }
}

void Socket::listenOrDie()
{
    int ret = ::listen(sockfd_, SOMAXCONN);
    if(ret)
    {
        perror("Socket::listen");
        abort();
    }
}

Socket Socket::createTCP()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC,IPPROTO_TCP);
    assert(sockfd>=0);
    return Socket(sockfd);
}

int Socket::write(const void *buf, int len)
{
    return ::write(sockfd_, buf, len);
}

int Socket::connect(const InetAddress &addr)
{
    const struct sockaddr_in& saddr = addr.getSockAddrInet();
    return ::connect(sockfd_, sockaddr_cast(&saddr), sizeof saddr);
}

void Socket::shutdownWrite()
{
    if (::shutdown(sockfd_, SHUT_WR) < 0)
    {
        perror("Socket::shutdownWrite");
    }
}