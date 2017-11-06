//
// Created by root on 17-10-24.
//
#include "../InetAddress.h"
#include "../../base/Logging.h"
//#define BOOST_TEST_MODULE InetAddressTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(testInetAddressResolve)
{
    InetAddress addr(80);
    if (InetAddress::resolve("baidu.com", &addr))
    {
        LOG_INFO << "google.com resolved to " << addr.toIpPort();
    }
    else
    {
        LOG_ERROR << "Unable to resolve google.com";
    }
}
