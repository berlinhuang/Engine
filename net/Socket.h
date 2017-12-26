//
// Created by root on 17-11-24.
//

#ifndef ENGINE_SOCKET_H
#define ENGINE_SOCKET_H

class InetAddress;

class Socket {
public:
    explicit Socket(int sockfd)
    :sockfd_(sockfd)
    {

    }

    ~Socket();

    int fd() const { return sockfd_;}

    void bindAddress(const InetAddress& lockaddr);

    void listen();

    int accept(InetAddress* peeraddr);

    void setReuseAddr(bool on);

    void setReusePort(bool on);

    void shutdownWrite();

    void setTcpNoDelay(bool on);

    void setKeepAlive(bool on);

private:
    const int sockfd_;
};


#endif //ENGINE_SOCKET_H
