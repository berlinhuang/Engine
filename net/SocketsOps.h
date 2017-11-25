//
// Created by root on 17-11-6.
//

#ifndef ENGINE_SOCKETSOPS_H
#define ENGINE_SOCKETSOPS_H

#include <arpa/inet.h> //size_t uint16_t

namespace sockets
{

    void listenOrDie(int sockfd);
    void bindOrDie(int sockfd, const struct sockaddr* addr);
    int  accept(int sockfd, struct sockaddr_in6* addr);
    void close(int sockfd);

    void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
    void toIp(char* buf, size_t size, const struct sockaddr* addr);

    void fromIpPort( const char* ip, uint16_t port, struct sockaddr_in* addr);
    void fromIpPort( const char* ip, uint16_t port, struct sockaddr_in6* addr);

    int getSocketError(int sockfd);

    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
    struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);

    const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
    const struct sockaddr_in6* sockaddr_in6_cast( const struct sockaddr* addr);



    ssize_t write(int sockfd, const void *buf, size_t count);


    int createNonblockingOrDie(sa_family_t family);

    struct sockaddr_in6 getLocalAddr(int sockfd);

}



#endif //ENGINE_SOCKETSOPS_H


