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

int TcpStream::receiveAll(void *buf, int len)
{
    return ::recv(sock_.fd(), buf, len, MSG_WAITALL);
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

TcpStreamPtr TcpStream::connect(const InetAddress& serverAddr)
{
    return connectInternal(serverAddr, nullptr);
}

TcpStreamPtr TcpStream::connect(const InetAddress& serverAddr, const InetAddress& localAddr)
{
    return connectInternal(serverAddr, &localAddr);
}

TcpStreamPtr TcpStream::connectInternal(const InetAddress& serverAddr, const InetAddress* localAddr)
{
    TcpStreamPtr stream;
    Socket sock(Socket::createTCP());
    if (localAddr)
    {
        sock.bindOrDie(*localAddr);
    }
    if (sock.connect(serverAddr) == 0 && !isSelfConnection(sock))
    {
        // FIXME: do poll(POLLOUT) to check errors
        stream.reset(new TcpStream(std::move(sock)));
    }
    return stream;
}

void TcpStream::shutdownWrite()
{
    sock_.shutdownWrite();
}