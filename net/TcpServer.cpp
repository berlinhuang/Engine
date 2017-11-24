//
// Created by root on 17-11-24.
//

#include "TcpServer.h"
#include "InetAddress.h"
#include "./EventLoop.h"
#include "SocketsOps.h"
#include "TcpConnection.h"
#include "./../base/Logging.h"

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf,sizeof buf, "#%d",nextConnId_);
    ++nextConnId_;
    std::string connName = name_+buf;

    LOG_INFO<<"TcpServer::newConnection["<<name_
            <<"] -new connection ["<<connName
            <<"] from "<<peerAddr.toIpPort();
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(
            new TcpConnection(loop_,connName, sockfd, localAddr, peerAddr));
    connections_[connName]=conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->connectEstablished();
}