//
// Created by root on 18-8-1.
//
#include <sys/socket.h>
#include "TcpStream.h"


namespace
{
    class IgnoreSigPipe
    {
    public:

    }initObj;

    bool isSelfConnection(const Socket& sock)
    {

    }
}

TcpStream::TcpStream(Socket&& sock)
        :sock_(std::move(sock))
{

}

void TcpStream::setTcpNoDelay(bool on)
{
    sock_.setTcpNoDelay(on);
}


int TcpStream::receiveSome(void *buf, int len)
{
    return ::recv(sock_.fd(),buf,len,MSG_WAITALL);
}

int TcpStream::sendAll(const void *buf, int len)
{
    int written = 0;
    while(written<len)
    {
        int nw = sock_.write(static_cast<const char*>(buf)+written,len-written);
        if(nw>0)
        {
            written+=nw;
        }
        else if(nw == 0)
        {
            break;
        }
        else if(errno !=EINTR)//nw<0
        {
            break;
        }
    }
    return written;
}
