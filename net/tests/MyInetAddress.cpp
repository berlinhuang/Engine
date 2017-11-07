//
// Created by root on 17-10-24.
//
#include "../InetAddress.h"
#include "../../base/Logging.h"
//#define BOOST_TEST_MODULE InetAddressTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK   // 引入boost test

/**
 * #define BOOST_TEST_DYN_LINK 如果不加的话会出现下面的错误：
 * Undefined symbols for architecture x86_64:
    "_main", referenced from:
        implicit entry/start for main executable
   ld: symbol(s) not found for architecture x86_64
   clang: error: linker command failed with exit code 1 (use -v to see invocation)

   编译时同时需要链接-lboost_unit_test_framework
   target_link_libraries(MyInetAddress boost_unit_test_framework)
 */



#include <boost/test/unit_test.hpp>



/**
 * 测试工具分为"WARN”，“CHECK"和"REQUIRE"三个等级
 * “CHECK"与"REQUIRE"差别为：前者即使失败，也仍然继续；后者则认为是必须的，为严重错误，直接退出当前测试
 */

BOOST_AUTO_TEST_CASE(testInetAddress)
{
    InetAddress addr0(1234);
    LOG_INFO<<addr0.toIp();
    LOG_INFO<<addr0.toIpPort();
    BOOST_CHECK_EQUAL(addr0.toIp(),string("0.0.0.0"));
    BOOST_CHECK_EQUAL(addr0.toIpPort(), string("0.0.0.0:1234"));
    BOOST_CHECK_EQUAL(addr0.toPort(),1234);

    InetAddress addr1(4321,true);
    LOG_INFO<<addr1.toIp();
    LOG_INFO<<addr1.toIpPort();
    BOOST_CHECK_EQUAL(addr1.toIp(),string("127.0.0.1"));
    BOOST_CHECK_EQUAL(addr1.toIpPort(), string("127.0.0.1:4321"));
    BOOST_CHECK_EQUAL(addr1.toPort(), 4321);

    InetAddress addr2("1.2.3.4", 8888);
    LOG_INFO<<addr2.toIp();
    LOG_INFO<<addr2.toIpPort();
    BOOST_CHECK_EQUAL(addr2.toIp(), string("1.2.3.4"));
    BOOST_CHECK_EQUAL(addr2.toIpPort(), string("1.2.3.4:8888"));
    BOOST_CHECK_EQUAL(addr2.toPort(), 8888);


    InetAddress addr3("255.254.253.252", 65535);
    LOG_INFO<<addr3.toIp();
    LOG_INFO<<addr3.toIpPort();
    BOOST_CHECK_EQUAL(addr3.toIp(), string("255.254.253.252"));
    BOOST_CHECK_EQUAL(addr3.toIpPort(), string("255.254.253.252:65535"));
    BOOST_CHECK_EQUAL(addr3.toPort(), 65535);


}



BOOST_AUTO_TEST_CASE(testInetAddressResolve) // 自动注册测试用例
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
