//
// Created by root on 17-9-13.
//
#include <unistd.h>
#include "./../ThreadPool.h"
#include "./../Logging.h"

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
//    pool.run(logInThread);
//    pool.run(logInThread);
//    pool.run(logInThread);
//    pool.run(logInThread);
//
//    LOG_TRACE<<"trace";
}