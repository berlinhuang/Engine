//
// Created by root on 17-12-21.
//

#ifndef ENGINE_CODEC_H
#define ENGINE_CODEC_H

#include <google/protobuf/message.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "./../../../net/Buffer.h"
#include "./../../../net/TcpConnection.h"



typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;



class ProtobufCodec
{
public:

    enum ErrorCode
    {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    typedef boost::function<void (const TcpConnectionPtr&, const MessagePtr&, Timestamp)> ProtobufMessageCallback;

    typedef boost::function<void (const TcpConnectionPtr&, Buffer*, Timestamp, ErrorCode)> ErrorCallback;

    explicit ProtobufCodec(const ProtobufMessageCallback& messageCb)
            : messageCallback_(messageCb),
              errorCallback_(defaultErrorCallback)
    {
    }

    ProtobufCodec(const ProtobufMessageCallback& messageCb, const ErrorCallback& errorCb)
            : messageCallback_(messageCb),
              errorCallback_(errorCb)
    {
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);

    void send(const TcpConnectionPtr& conn, const google::protobuf::Message& message)
    {
        // FIXME: serialize to TcpConnection::outputBuffer()
        Buffer buf;
        fillEmptyBuffer(&buf, message);
        conn->send(&buf);
    }

    static const string& errorCodeToString(ErrorCode errorCode);
    static void fillEmptyBuffer(Buffer* buf, const google::protobuf::Message& message);
    static google::protobuf::Message* createMessage(const std::string& type_name);
    static MessagePtr parse(const char* buf, int len, ErrorCode* errorCode);

private:
    static void defaultErrorCallback(const TcpConnectionPtr&, Buffer*, Timestamp, ErrorCode);

    ProtobufMessageCallback messageCallback_;
    ErrorCallback errorCallback_;

    const static int kHeaderLen = sizeof(int32_t);
    const static int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
    const static int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit
};

#endif //ENGINE_CODEC_H
