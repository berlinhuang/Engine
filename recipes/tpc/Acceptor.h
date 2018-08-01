//
// Created by root on 18-8-1.
//

#ifndef ENGINE_ACCPTOR_H
#define ENGINE_ACCPTOR_H


#pragma once
#include "Common.h"
#include "Socket.h"

#include <memory>

class InetAddress;

class TcpStream;
typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class Acceptor : noncopyable
{
public:
    explicit Acceptor(const InetAddress& listenAddr);

    ~Acceptor() = default;
    Acceptor(Acceptor&&) = default;
    Acceptor& operator=(Acceptor&&) = default;

    // thread safe
    TcpStreamPtr accept();
    Socket acceptSocketOrDie();

private:
    Socket listenSock_;
};

#endif //ENGINE_ACCPTOR_H
