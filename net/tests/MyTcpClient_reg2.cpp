//
// Created by root on 17-12-7.
//

#include "./../../base/Thread.h"
#include "./../../base/Logging.h"
#include "./../EventLoop.h"
#include "./../TcpClient.h"

#include <boost/bind.hpp>


void threadFunc(EventLoop* loop)
{
    InetAddress serverAddr("127.0.0.1", 1234); // should succeed

    TcpClient client(loop, serverAddr, "TcpClient");
    client.connect();

    CurrentThread::sleepUsec(1000*1000);
    // client destructs when connected.
}

int main(int argc, char* argv[])
{
    Logger::setLogLevel(Logger::DEBUG);

    EventLoop loop;
    loop.runAfter(3.0, boost::bind(&EventLoop::quit, &loop));
    Thread thr(boost::bind(threadFunc, &loop));
    thr.start();
    loop.loop();
}
