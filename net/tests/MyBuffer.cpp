//
// Created by root on 17-11-28.
//

//#define BOOST_TEST_MODULE BufferTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "./../Buffer.h"

#include <string>

#include <stdio.h>

void print(Buffer* buf, std::string str)
{
    printf("-----------%s-----------\n",str.c_str());
    printf("readableBytes =%zu\n",buf->readableBytes());
    printf("writableBytes =%zu\n",buf->writableBytes());
    printf("prependableBytes =%zu\n",buf->prependableBytes());
}



BOOST_AUTO_TEST_CASE(testBufferAppendRetrieve)
{
    ///    8    0     1024
    /// 0----rw=8-------size=1032
    Buffer buf;
    BOOST_CHECK_EQUAL(buf.readableBytes(),0);
    BOOST_CHECK_EQUAL(buf.writableBytes(),Buffer::kInitialSize);
    BOOST_CHECK_EQUAL(buf.prependableBytes(),Buffer::kCheapPrepend);
    print(&buf,"init");

    ///    8      200         824
    /// 0----r=8------w=208-------size=1032
    const std::string str(200,'x');
    buf.append(str);
    BOOST_CHECK_EQUAL(buf.readableBytes(), str.size());
    BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize - str.size());
    BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);
    print(&buf,"append 200");

    /// hua dong chuang kou

    ///    38      150         824
    /// 0----r=58------w=208-------size=1032
    const std::string str2 = buf.retrieveAsString(50);
    BOOST_CHECK_EQUAL(str2.size(), 50);
    BOOST_CHECK_EQUAL(buf.readableBytes(), str.size() - str2.size());
    BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize - str.size());
    BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend + str2.size());
    BOOST_CHECK_EQUAL(str2, string(50, 'x'));
    print(&buf, "retrieveAsString");

    ///    38      350         624
    /// 0----r=58------w=408-------size=1032
    buf.append(str);
    BOOST_CHECK_EQUAL(buf.readableBytes(),2*str.size()-str2.size());
    BOOST_CHECK_EQUAL(buf.writableBytes(),Buffer::kInitialSize-2*str.size());
    BOOST_CHECK_EQUAL(buf.prependableBytes(),Buffer::kCheapPrepend+str2.size());
    BOOST_CHECK_EQUAL(str2, string(50,'x'));
    print(&buf, "append 200");


    /// retrieveall will reset
    ///    8    0     1024
    /// 0----rw=8-------size=1032
    const string str3 = buf.retrieveAllAsString();
    BOOST_CHECK_EQUAL(str3.size(), 350);
    BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
    BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize);
    BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);
    BOOST_CHECK_EQUAL(str3, string(350, 'x'));
    print(&buf, "retrieveAllAsString");

}


BOOST_AUTO_TEST_CASE(testBufferGrow)
{

    ///    8      400         624
    /// 0----r=8------w=408-------size=1032
    Buffer buf;
    buf.append(string(400,'y'));
    BOOST_CHECK_EQUAL(buf.readableBytes(), 400);
    BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-400);
    print(&buf, "append 400");

    ///    58      350         624
    /// 0----r=58------w=408-------size=1032
    buf.retrieve(50);
    BOOST_CHECK_EQUAL(buf.readableBytes(), 350);
    BOOST_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitialSize-400);
    BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend+50);
    print(&buf, "retrieve 50");

    ///    58      read=350+1000     write=0
    /// 0----r=58--------------w=1408 size=1408
    buf.append(string(1000, 'z'));
    BOOST_CHECK_EQUAL(buf.readableBytes(), 1350);
    BOOST_CHECK_EQUAL(buf.writableBytes(), 0);
    BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend+50); // FIXME
    print(&buf, "append 1000");


    ///    8    0   1400
    /// 0----rw=8-------size=1048
    buf.retrieveAll();
    BOOST_CHECK_EQUAL(buf.readableBytes(), 0);
    BOOST_CHECK_EQUAL(buf.writableBytes(), 1400); // FIXME
    BOOST_CHECK_EQUAL(buf.prependableBytes(), Buffer::kCheapPrepend);
    print(&buf, "retrieveAll 1000");

}





