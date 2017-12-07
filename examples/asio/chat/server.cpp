//
// Created by root on 17-12-7.
//

#include "codec.h"

#include "./../../../base/Logging.h"
#include "./../../../base/Mutex.h"
#include "./../../../net/EventLoop.h"
#include "./../../../net/TcpServer.h"

#include <boost/bind.hpp>

#include <set>
#include <stdio.h>



class ChatServer
{
public:
    ChatServer(EventLoop* loop, const InetAddress& listenAddr)
            : server_(loop, listenAddr, "ChatServer"),//tcpserver                 socket           bind
              codec_(boost::bind(&ChatServer::onStringMessage, this, _1, _2, _3))//LengthHeaderCodec.messageCallback_ = ChatServer::onStringMessage
    {
        server_.setConnectionCallback(boost::bind(&ChatServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));//
    }

    void start()
    {
        server_.start();//listen
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            connections_.insert(conn);
        }
        else
        {
            connections_.erase(conn);
        }
    }

    void onStringMessage(const TcpConnectionPtr&, const string& message, Timestamp)// codec will callback
    {
        //循环 广播消息
        for (ConnectionList::iterator it = connections_.begin();
             it != connections_.end();
             ++it)
        {
            codec_.send(get_pointer(*it), message);////把string message 打包为 Buffer
        }
    }

    typedef std::set<TcpConnectionPtr> ConnectionList;
    ConnectionList connections_;


    TcpServer server_;


    LengthHeaderCodec codec_;
};

int main(int argc, char* argv[])
{
//    LOG_INFO << "pid = " << getpid();
//    if (argc > 1)
//    {
        EventLoop loop;
//        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        uint16_t port = 9981;
        InetAddress serverAddr(port);
        ChatServer server(&loop, serverAddr);
        server.start();
        loop.loop();
//    }
//    else
//    {
//        printf("Usage: %s port\n", argv[0]);
//    }
}

