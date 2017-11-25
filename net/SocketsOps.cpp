//
// Created by root on 17-11-6.
//
#include <string.h>
#include <unistd.h>
#include <stdio.h>  // snprintf
//#include <fcntl.h>
#include <assert.h>

#include "SocketsOps.h"
#include "Endian.h"
#include "./../base/Type.h"
#include "./../base/Logging.h"

namespace sockets
{

    void listenOrDie(int sockfd)
    {
        int ret = ::listen(sockfd, SOMAXCONN);
        if (ret < 0)
        {
            LOG_SYSFATAL << "sockets::listenOrDie";
        }
    }

    void bindOrDie(int sockfd, const struct sockaddr* addr)
    {
        int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
        if (ret < 0)
        {
            LOG_SYSFATAL << "sockets::bindOrDie";
        }
    }

    int accept(int sockfd, struct sockaddr_in6* addr)
    {
        socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
#if VALGRIND || defined (NO_ACCEPT4)
        int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
  setNonBlockAndCloseOnExec(connfd);
#else
        int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
        if (connfd < 0)
        {
            int savedErrno = errno;
            LOG_SYSERR << "Socket::accept";
            switch (savedErrno)
            {
                case EAGAIN:
                case ECONNABORTED:
                case EINTR:
                case EPROTO: // ???
                case EPERM:
                case EMFILE: // per-process lmit of open file desctiptor ???
                    // expected errors
                    errno = savedErrno;
                    break;
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENFILE:
                case ENOBUFS:
                case ENOMEM:
                case ENOTSOCK:
                case EOPNOTSUPP:
                    // unexpected errors
                    LOG_FATAL << "unexpected error of ::accept " << savedErrno;
                    break;
                default:
                    LOG_FATAL << "unknown error of ::accept " << savedErrno;
                    break;
            }
        }
        return connfd;
    }

    void close(int sockfd)
    {
        if (::close(sockfd) < 0)
        {
            LOG_SYSERR << "sockets::close";
        }
    }


    void toIp(char* buf, size_t size, const struct sockaddr* addr)
    {
        if (addr->sa_family == AF_INET)
        {
            assert(size >= INET_ADDRSTRLEN);
            const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
            ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
        }
        else if (addr->sa_family == AF_INET6)
        {
            assert(size >= INET6_ADDRSTRLEN);
            const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
            ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
        }
    }


    void toIpPort(char *buf, size_t size, const struct sockaddr *addr)
    {
        toIp(buf,size,addr);
        size_t end = ::strlen(buf);
        const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
        uint16_t port = networkToHost16(addr4->sin_port);
        assert(size>end);
        snprintf(buf+end,size-end,":%u",port);
    }



    void fromIpPort( const char* ip, uint16_t port, struct sockaddr_in* addr)
    {
        addr->sin_family = AF_INET;
        addr->sin_port = hostToNetwork16(port);//主机序转成网络字节序
        if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)//presentation to numeric
        {
            LOG_SYSERR << "sockets::fromIpPort";
        }
    }

    void fromIpPort( const char* ip, uint16_t port, struct sockaddr_in6* addr)
    {
        addr->sin6_family = AF_INET6;
        addr->sin6_port = hostToNetwork16(port);
        if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
        {
            LOG_SYSERR << "sockets::fromIpPort";
        }
    }

    int getSocketError(int sockfd)
    {
        int optval;
        socklen_t optlen = static_cast<socklen_t>(sizeof optval);

        if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        {
            return errno;
        }
        else
        {
            return optval;
        }
    }



    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr)
    {
        return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
    }
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr)
    {
        return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
    }
    struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr)
    {
        return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
    }



    const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr)
    {
        return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
    }
    const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr)
    {
        return static_cast<const struct sockaddr_in6*>(implicit_cast<const void*>(addr));
    }


    ssize_t write(int sockfd, const void *buf, size_t count)
    {
        return ::write(sockfd, buf, count);
    }


    int createNonblockingOrDie(sa_family_t family)
    {
#if VALGRIND
        int sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0)
  {
    LOG_SYSFATAL << "sockets::createNonblockingOrDie";
  }

  setNonBlockAndCloseOnExec(sockfd);
#else
        int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
        if (sockfd < 0)
        {
            LOG_SYSFATAL << "sockets::createNonblockingOrDie";
        }
#endif
        return sockfd;
    }



    struct sockaddr_in6 getLocalAddr(int sockfd)
    {
        struct sockaddr_in6 localaddr;
        bzero(&localaddr, sizeof localaddr);
        socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
        if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
        {
            LOG_SYSERR << "sockets::getLocalAddr";
        }
        return localaddr;
    }

}