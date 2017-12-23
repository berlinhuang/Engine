//
// Created by root on 17-10-24.
//

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

//     struct sockaddr_in6 {
//         sa_family_t     sin6_family;   /* address family: AF_INET6 */
//         uint16_t        sin6_port;     /* port in network byte order */
//         uint32_t        sin6_flowinfo; /* IPv6 flow information */
//         struct in6_addr sin6_addr;     /* IPv6 address */
//         uint32_t        sin6_scope_id; /* IPv6 scope-id */
//     };

// sa_family_t:
// IPv4                 -> AF_INET
// IPv6                 -> AF_INET6
// Unix Domain Socket   -> AF_UNIX

// struct sockaddr{
//    sa_family_t   sa_family;
//    char          sa_data[4];     /* 14 bytes of protocol address */
// };

// struct sockaddr_in
// struct sockaddr_in6





#include "InetAddress.h"
#include "./Endian.h"
#include "./SocketsOps.h"
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



InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6)
{
    if (ipv6)
    {
        bzero(&addr6_, sizeof addr6_);
        sockets::fromIpPort(ip.c_str(), port, &addr6_);
    }
    else
    {
        bzero(&addr_, sizeof addr_);
        sockets::fromIpPort(ip.c_str(), port, &addr_);
    }
}

uint32_t InetAddress::ipNetEndian() const
{
    assert(family() == AF_INET);
    return addr_.sin_addr.s_addr;
}

string InetAddress::toIp() const
{
    char buf[64] = "";
    sockets::toIp(buf,sizeof buf, getSockAddr());
    return buf;
}


string InetAddress::toIpPort() const
{
    char buf[64] = "";
    sockets::toIpPort(buf,sizeof buf, getSockAddr());
    return buf;
}

uint16_t  InetAddress::toPort() const
{
    return sockets::networkToHost16(portNetEndian());
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
