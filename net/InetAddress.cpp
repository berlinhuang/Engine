//
// Created by root on 17-10-24.
//

#include "InetAddress.h"

#include <boost/static_assert.hpp>

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    //offsetof(a,b)第一个参数是结构体的名字，第二个参数是结构体成员的名字 该宏返回结构体structName中成员memberName的偏移量。偏移量是size_t类型的
    BOOST_STATIC_ASSERT(offsetof(InetAddress, addr6_) == 0);
}