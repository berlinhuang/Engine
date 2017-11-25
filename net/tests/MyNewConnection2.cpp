//
// Created by root on 17-11-25.
//
#include "../Callbacks.h"
#include "../TcpConnection.h"
#include "../InetAddress.h"
#include "../EventLoop.h"
#include "../TcpServer.h"
#include "../../base/Logging.h"

void onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        printf("onConnection(): new connection [%s] from %s\n",
            conn->name().c_str(),
            conn->peerAddress().toIpPort().c_str());
    }
    else
    {
        printf("onConnection(): connection [%s] is down\n",
            conn->name().c_str());
    }
}



void onMessage(const TcpConnectionPtr& conn,
                const char*data,
                ssize_t len)
{
    printf("onMessage():received %zd bytes from connection [%s]\n",
        len,conn->name().c_str());
}


int main()
{

    Logger::setLogLevel(Logger::DEBUG);

    printf("main(): pid = %d\n",getpid());
    InetAddress listenAddr(9981);
    EventLoop loop;


    TcpServer server(&loop,listenAddr,"myServer");
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();
    loop.loop();


}
