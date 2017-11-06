//
// Created by root on 17-10-24.
//

#include "InetAddress.h"
#include "./Endian.h"
#include "../base/Logging.h"
#include "../base/Logging.h"
#include <boost/static_assert.hpp>

#include <assert.h>
#include <netdb.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"


InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    //offsetof(a,b)第一个参数是结构体的名字，第二个参数是结构体成员的名字 该宏返回结构体structName中成员memberName的偏移量。偏移量是size_t类型的
    BOOST_STATIC_ASSERT(offsetof(InetAddress, addr6_) == 0);
    BOOST_STATIC_ASSERT(offsetof(InetAddress, addr_) == 0);
    if (ipv6)
    {
        bzero(&addr6_, sizeof addr6_);
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = sockets::hostToNetwork16(port);
    }
    else
    {
        bzero(&addr_, sizeof addr_);
        addr_.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        addr_.sin_port = sockets::hostToNetwork16(port);
    }
}


string InetAddress::toIpPort() const
{
    char buf[64] = "";
    sockets::toIpPort(buf,sizeof buf, getSockAddr());
    return buf;
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(StringArg hostname, InetAddress* out)
{
    assert(out != NULL);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != NULL)
    {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else
    {
        if (ret)
        {
            LOG_SYSERR << "InetAddress::resolve";
        }
        return false;
    }
}
