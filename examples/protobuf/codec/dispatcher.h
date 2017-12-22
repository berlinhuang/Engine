//
// Created by root on 17-12-21.
//

#ifndef ENGINE_DISPATCHER_H
#define ENGINE_DISPATCHER_H


#include <google/protobuf/message.h>

#include <map>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#ifndef NDEBUG
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
#endif

#include "./../../../net/Callbacks.h"
#include "./../../../base/Timestamp.h"



typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

class Callback
{
public:
    virtual ~Callback(){};
    virtual void onMessage(const TcpConnectionPtr& , const MessagePtr& message, Timestamp) const = 0;
};



template <typename T>
class CallbackT : public Callback
{
#ifndef NDEBUG
    BOOST_STATIC_ASSERT((boost::is_base_of<google::protobuf::Message, T>::value));
#endif
public:
    typedef boost::function<void (const TcpConnectionPtr&, const boost::shared_ptr<T>& message, Timestamp)> ProtobufMessageTCallback;

    CallbackT(const ProtobufMessageTCallback& callback)
            : callback_(callback)
    {
    }

    virtual void onMessage(const TcpConnectionPtr& conn, const MessagePtr& message, Timestamp receiveTime) const
    {
        boost::shared_ptr<T> concrete = down_pointer_cast<T>(message);//消息类型转换
        assert(concrete != NULL);
        callback_(conn, concrete, receiveTime);
    }

private:
    ProtobufMessageTCallback callback_;
};


class ProtobufDispatcher
{
public:
    typedef boost::function<void (const TcpConnectionPtr&, const MessagePtr& message, Timestamp)> ProtobufMessageCallback;
    explicit  ProtobufDispatcher( const ProtobufMessageCallback& defaultCb)
            :defaultCallback_(defaultCb)
    {

    }

    /**
     *  codec反序列化后调用
     *  向上类型转换找到消息类型 根据每个类型的descriptor来从一张表(map callbacks_)中查找处理函数
     * @param conn
     * @param message
     * @param receiveTime
     */
    void onProtobufMessage(const TcpConnectionPtr& conn, const MessagePtr& message, Timestamp receiveTime) const
    {
        CallbackMap::const_iterator it = callbacks_.find(message->GetDescriptor());
        if (it != callbacks_.end())
        {
            it->second->onMessage(conn, message, receiveTime);//Callback::onMessage
        }
        else
        {
            defaultCallback_(conn, message, receiveTime);//找不到，就调用 defaultCallback
        }
    }


  /**
   * 注册相应的具体类型的回调函数
   * @tparam T
   * @param callback
   */
    // T Engine::Query Engine::Answer
    // Server.cpp    dispatcher_.registerMessageCallback<Engine::Query>( boost::bind(&QueryServer::onQuery, this, _1, _2, _3));
    template<typename T>
    void registerMessageCallback(const typename CallbackT<T>::ProtobufMessageTCallback& callback)
    {
        boost::shared_ptr<CallbackT<T> > pd(new CallbackT<T>(callback));//根据回调生成相应的function
        callbacks_[T::descriptor()] = pd;//注册
    }


private:
    //映射  Descriptor => Callback
    typedef std::map<const google::protobuf::Descriptor*, boost::shared_ptr<Callback> > CallbackMap;
    CallbackMap callbacks_;
    ProtobufMessageCallback defaultCallback_;
};


#endif //ENGINE_DISPATCHER_H







