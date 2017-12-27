//
// Created by root on 17-12-7.
//

#ifndef ENGINE_CONNECTOR_H
#define ENGINE_CONNECTOR_H

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include "InetAddress.h"




class Channel;
class EventLoop;


class Connector: public boost::enable_shared_from_this<Connector>
{
public:
    typedef boost::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }

    void start();
    void restart();
    void stop();

    const InetAddress& serverAddress()const
    {
        return serverAddr_;
    }

private:
    enum States{ kDisconnected, kConnecting, kConnected};
    static const int kMaxRetryDelayMs = 30*1000;   //默认最大重连时间30000ms
    static const int kInitRetryDelayMs = 500;  //默认重连延迟时间500ms

    void setState(States s) { state_ = s; }

    void startInLoop();
    void stopInLoop();

    void connect();
    void connecting(int sockfd);

    int removeAndResetChannel();
    void resetChannel();

    void handleWrite();
    void handleError();

    void retry(int sockfd);


    EventLoop* loop_;
    InetAddress serverAddr_;
    bool connect_;
    States state_;
    boost::scoped_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;


};


#endif //ENGINE_CONNECTOR_H
