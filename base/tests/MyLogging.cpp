//
// Created by root on 17-9-13.
//
#include <unistd.h>
#include "./../ThreadPool.h"
#include "./../Logging.h"
#include <stdio.h>

void logInThread()
{
    LOG_INFO<<"logInThread";
    usleep(1000);
}


int main()
{
    getppid();

    ThreadPool pool("pool");
    pool.start(5);

    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);

    LOG_TRACE<<"trace";
    LOG_DEBUG << "debug";
    LOG_INFO << "Hello";
    LOG_WARN << "World";
    LOG_ERROR << "Error";

    LOG_INFO << sizeof(Logger);
    LOG_INFO << sizeof(LogStream);
    LOG_INFO << sizeof(Fmt);
    LOG_INFO << sizeof(LogStream::Buffer);
}