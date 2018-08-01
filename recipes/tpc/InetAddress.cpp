//
// Created by root on 18-8-1.
//

#include "InetAddress.h"

#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <netdb.h>

InetAddress::InetAddress(uint16_t port, bool loopbackOnly)
{
    ::bzero(&saddr_, sizeof(saddr_));
    saddr_.sin_family = AF_INET;
    saddr_.sin_addr.s_addr = htonl(loopbackOnly? INADDR_LOOPBACK:INADDR_ANY);
    saddr_.sin_port = htons(port);
}

InetAddress::InetAddress(StringArg ip, uint16_t port)
:InetAddress(port, false)
{
    if(::inet_pton(AF_INET,ip.c_str(),&saddr_.sin_addr)!=1)
        assert("Invalid IP format");
}

static const int kResolveBufSize = 4096;

/*
struct hostent
{
    char *h_name;			// Official name of host. 主机的规范名。例如www.google.com的规范名其实是www.l.google.com
    char **h_aliases;		// Alias list. www.google.com就是google他自己的别名
    int h_addrtype;		    // Host address type. 主机ip地址的类型，到底是ipv4(AF_INET)，还是ipv6(AF_INET6)
    int h_length;			// Length of address. 主机ip地址的长度
    char **h_addr_list;		// List of addresses from name server. 主机的ip地址，注意，这个是以网络字节序存储的。千万不要直接用printf带%s参数来打这个东西，会有问题的哇。所以到真正需要打印出这个IP的话，需要调用inet_ntop()。
    #ifdef __USE_MISC
    # define	h_addr	h_addr_list[0] // Address, for backward compatibility.
    #endif
};
 */
bool InetAddress::resolveSlow(const char* hostname, InetAddress* out)
{
    std::vector<char> buf(2 * kResolveBufSize);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    while (buf.size() <= 16 * kResolveBufSize)  // 64 KiB
    {
        int ret = gethostbyname_r(hostname, &hent, buf.data(), buf.size(), &he, &herrno);
        if (ret == 0 && he != NULL)
        {
            assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
            out->saddr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
            return true;
        }
        else if (ret == ERANGE)
        {
            buf.resize(buf.size() * 2);
        }
        else
        {
            if (ret)
                perror("InetAddress::resolve");
            return false;
        }
    }
    return false;
}

bool InetAddress::resolve(StringArg hostname, InetAddress* out)
{
    assert(out);
    char buf[kResolveBufSize];
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, buf, sizeof buf, &he, &herrno);
    if (ret == 0 && he != NULL)
    {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        out->saddr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else if (ret == ERANGE)
    {
        return resolveSlow(hostname.c_str(), out);
    }
    else
    {
        if (ret)
            perror("InetAddress::resolve");
        return false;
    }
}

std::string InetAddress::toIpPort() const
{
    char buf[32] = ""; // "255.255.255.255:65535" 4*4+5+1 = 22
    ::inet_ntop(AF_INET, &saddr_.sin_addr, buf, sizeof buf);
    int end = ::strlen(buf);
    uint16_t port = portHostEndian();
    snprintf(buf + end, sizeof buf - end, ":%u", port);
    return buf;
}

//getSockAddrInet()