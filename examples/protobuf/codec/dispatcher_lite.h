//
// Created by root on 17-12-22.
//

#ifndef ENGINE_DSIPATCHER_LITE_H
#define ENGINE_DSIPATCHER_LITE_H

#include "../../../net/Callbacks.h"

#include <google/protobuf/message.h>

#include <map>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

class ProtobufDispatcherLite : boost::noncopyable
{
public:
    typedef boost::function<void (const TcpConnectionPtr&, const MessagePtr&, Timestamp)> ProtobufMessageCallback;

    // ProtobufDispatcher()
    //   : defaultCallback_(discardProtobufMessage)
    // {
    // }

    explicit ProtobufDispatcherLite(const ProtobufMessageCallback& defaultCb)
            : defaultCallback_(defaultCb)
    {
    }

    void onProtobufMessage(TcpConnectionPtr& conn, const MessagePtr& message, Timestamp receiveTime) const
    {
        CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
        if (it != callbacks_.end())
        {
            it->second(conn, message, receiveTime);
        }
        else
        {
            defaultCallback_(conn, message, receiveTime);
        }
    }

    void registerMessageCallback(const google::protobuf::Descriptor* desc, const ProtobufMessageCallback& callback)
    {
        callbacks_[desc] = callback;
    }

private:
    // static void discardProtobufMessage(TcpConnectionPtr&,
    //                                    const MessagePtr&,
    //                                    Timestamp);

    typedef std::map<const google::protobuf::Descriptor*, ProtobufMessageCallback> CallbackMap;
    CallbackMap callbacks_;
    ProtobufMessageCallback defaultCallback_;
};
#endif //ENGINE_DSIPATCHER_LITE_H
