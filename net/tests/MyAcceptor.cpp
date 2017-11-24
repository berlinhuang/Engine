//
// Created by root on 17-11-24.
//
#include "./../InetAddress.h"
#include "./../EventLoop.h"
#include "./../Acceptor.h"

#include <stdio.h>

void newConnection(int sockfd, const InetAddress& peerAddr)
{
    printf("newConnection(): accepted a new connection from %s\n", peerAddr.toIpPort().c_str());
    ::write(sockfd, "how are you?\n",12);
    sockets::close(sockfd);
}





int main()
{
    printf("main():pid = %d\n",getpid());
    InetAddress listenAddr(9981);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();
}