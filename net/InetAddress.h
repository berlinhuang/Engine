//
// Created by root on 17-10-24.
//

#ifndef ENGINE_INETADDRESS_H
#define ENGINE_INETADDRESS_H

#include <cstdint>
#include <netinet/in.h>
#include "./../base/StringPiece.h"
#include "./SocketsOps.h"

namespace sockets
{
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr); //SocketsOps.ccp
}



class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false );
    InetAddress(StringArg ip, uint16_t, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr):addr_(addr){}
    explicit InetAddress(const struct sockaddr_in6& addr6):addr6_(addr6){}

    sa_family_t family() const { return addr_.sin_family;}
    string toIp() const;
    string toIpPort() const;
    uint16_t  toPort() const;

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }
    void setSockAddrInet6(const struct sockaddr_in6& addr6){ addr6_ = addr6; }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return addr_.sin_port;}

    static bool resolve(StringArg hostname, InetAddress* result);

private:
    union
    {
        struct sockaddr_in addr_;
        struct sockaddr_in6 addr6_;
    };
};


#endif //ENGINE_INETADDRESS_H
