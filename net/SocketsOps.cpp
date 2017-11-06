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
namespace sockets
{
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
}