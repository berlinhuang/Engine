//
// Created by root on 17-11-28.
//

//#define BOOST_TEST_MODULE BufferTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "./../Buffer.h"


#include <stdio.h>

BOOST_AUTO_TEST_CASE(testBufferAppendRetrieve)
{
    Buffer buf;
    printf("%zu\n",buf.readableBytes());
    BOOST_CHECK_EQUAL(buf.readableBytes(),0);
    BOOST_CHECK_EQUAL(buf.writableBytes(),Buffer::kInitialSize);
    BOOST_CHECK_EQUAL(buf.prependableBytes(),Buffer::kCheapPrepend);

    const std::string str(200,'x');
    buf.append(str);
    printf("re =%zu\n",buf.readableBytes());
    printf("wr =%zu\n",buf.writableBytes());
    printf("pr =%zu\n",buf.prependableBytes());


    //const std::string = buf.retrieveAsString(50);

}