//
// Created by root on 17-11-21.
//

#ifndef ENGINE_CALLBACKS_H_H
#define ENGINE_CALLBACKS_H_H

#include "./../base/Timestamp.h"
#include <boost/function.hpp>
class Buffer;
class TcpConnection;
typedef boost::function<void()> TimerCallback;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef boost::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef boost::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;



void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
                            Buffer* buffer,
                            Timestamp receiveTime);


//typedef boost::function<void (const TcpConnectionPtr&)>



#endif //ENGINE_CALLBACKS_H_H
