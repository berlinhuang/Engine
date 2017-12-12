//
// Created by root on 17-11-25.
//
#include <stdio.h>
#include "./../InetAddress.h"
#include "./../EventLoop.h"
#include "./../Acceptor.h"
#include "./../../base/Logging.h"

void newConnection(int sockfd, const InetAddress& peerAddr)
{
    printf("newConnection(): accepted a new connection from %s\n",
        peerAddr.toIpPort().c_str());

    ::write(sockfd, "how are you?\n",13);
    sockets::close(sockfd);
}


int main()
{

    Logger::setLogLevel(Logger::DEBUG);

    printf("main():pid = %d\n",getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;
    Acceptor acceptor(&loop, listenAddr, false);// socket            bind

    acceptor.setNewConnectionCallback(newConnection);

    acceptor.listen();// listen
    loop.loop(); // accepted

}