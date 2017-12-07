//
// Created by root on 17-12-7.
//

#ifndef ENGINE_CODEC_H
#define ENGINE_CODEC_H

#include "./../../../net/TcpConnection.h"
#include "./../../../net/Buffer.h"
#include "./../../../net/Endian.h"
#include "./../../../base/Logging.h"

#include <boost/function.hpp>

class LengthHeaderCodec
{
public:
    //typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef boost::function<void (const TcpConnectionPtr&, const string& message, Timestamp)> StringMessageCallback;
    explicit LengthHeaderCodec(const StringMessageCallback& cb):messageCallback_(cb){}


//对TCP粘包问题的处理
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
    {
        while(buf->readableBytes()>= kHeaderLen)////  kHeaderLen == 4 判断是否超过包头，如果包头都超不过，那半个消息都算不上
        {
            const void* data = buf->peek();//begin addr of data
            int32_t  be32 = *static_cast<const int32_t *>(data); ////转化成32位
            const int32_t len = sockets::networkToHost32(be32);
            if( len>65536 ||len<0)//2^16 = 2^10*64 = 64k 如果消息超过64K，或者长度小于0，不合法，干掉它。
            {
                LOG_ERROR << "Invalid length " << len;
                conn->shutdown();
                break;
            }
            else if( buf->readableBytes() >= len+kHeaderLen)
            { //如果缓冲区可读的数据是否>=len+head，说明是一条完整的消息，
              //取走len是头部规定的体部长度
                buf->retrieve(kHeaderLen);//get head
                string message(buf->peek(),len);// get body
                messageCallback_(conn, message,receiveTime); //call back
                buf->retrieve(len);
            }
            else//未达到一条完整的消息
            {
                break;//
            }
        }
    }

    void send(TcpConnection* conn, const StringPiece& message)
    {
        Buffer buf;
        buf.append(message.data(),message.size());//把string message 打包为 Buffer
        int32_t  len = static_cast<int32_t>(message.size());
        int32_t be32 = sockets::hostToNetwork32(len);
        buf.prepend(&be32, sizeof be32);//每条消息都有一个是个直接的头部
        conn->send(&buf);
    }


private:
    StringMessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};
#endif //ENGINE_CODEC_H
