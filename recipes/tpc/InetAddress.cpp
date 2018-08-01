//
// Created by root on 18-8-1.
//

#include "InetAddress.h"

#include <arpa/inet.h>
#include <assert.h>
#include <string.h>

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

//getSockAddrInet()