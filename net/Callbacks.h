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


typedef boost::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;




#endif //ENGINE_CALLBACKS_H_H
