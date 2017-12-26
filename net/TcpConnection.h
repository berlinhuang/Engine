//
// Created by root on 17-11-24.
//

#ifndef ENGINE_TCPCONNECTION_H
#define ENGINE_TCPCONNECTION_H

#include "Callbacks.h"
#include "./InetAddress.h"// forward declare seems not working
#include "./Buffer.h"


#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/any.hpp>

//class InetAddress;
class EventLoop;
class Socket;
class Channel;


class TcpConnection:public boost::enable_shared_from_this<TcpConnection>
{

public:
    TcpConnection(EventLoop* loop,
                  const string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr
    );

    ~TcpConnection();


    EventLoop* getLoop() const { return loop_; }
    const string& name() const { return name_; }


    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }

    // when accept a new connection on TcpServer::newConnection
    void setConnectionCallback(const ConnectionCallback& cb)   { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb)    { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb)    { closeCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)    { writeCompleteCallback_ = cb;   }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
    {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    bool isReading() const { return reading_; };
    void setContext(const boost::any& context) { context_ = context; }
    boost::any* getMutableContext()   { return &context_; }

    const boost::any& getContext() const
    { return context_; }



    void connectEstablished();
    void connectDestroyed();

    void handleRead( Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();


    void forceClose();
    void forceCloseInLoop();

    void setTcpNoDelay(bool on);

    void shutdown();
    void shutdownInLoop();

    void startRead();
    void stopRead();


    void sendInLoop(const void* data, size_t len);
    void sendInLoop(const StringPiece& message);
    void send(const void* message, int len);
    void send(const StringPiece& message);
    void send(Buffer* buf);


    const char* stateToString() const;

    Buffer* inputBuffer()
    { return &inputBuffer_; }

    Buffer* outputBuffer()
    { return &outputBuffer_; }

private:


    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    StateE state_;  // FIXME: use atomic variable
    void setState(StateE s) { state_ = s; }

    void startReadInLoop();
    void stopReadInLoop();


    bool reading_;


    EventLoop* loop_;
    const string name_;

    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;


    const InetAddress localAddr_;
    const InetAddress peerAddr_;

    size_t highWaterMark_;



    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    // 数据发送完毕的回调函数 即所有的用户数据都拷贝到内核缓冲区回调该函数
    // 如果对等方接受不及时 受到通告窗口的控制 内核发送缓存不足 这个时候 就会将用户数据添加到应用层发送缓冲区 outbuffer
    // 可能会撑爆outbuffer 解决方法:调整发送频率 关注writeCompleteCallback

    // 将所有的数据都发送完 writeCompleteCallback回调 继续发送
    WriteCompleteCallback writeCompleteCallback_;// 低水位回调函数
    HighWaterMarkCallback highWaterMarkCallback_;// 高水位回调函数 outbuffer 快满了

    CloseCallback closeCallback_; // 内部的close回调函数

    // 应用层的接收和发送缓冲区
    Buffer inputBuffer_;
    Buffer outputBuffer_;

    boost::any context_;// boost的any库 可以保持任意的类型 绑定一个未知类型的上下文对象
};
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

#endif //ENGINE_TCPCONNECTION_H
