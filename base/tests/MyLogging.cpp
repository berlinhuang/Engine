//
// Created by root on 17-9-13.
//
#include <unistd.h>
#include "./../ThreadPool.h"
#include "./../Logging.h"
//#include "./../LogFile.h"
#include <stdio.h>
//boost::scoped_ptr<LogFile> g_logFile;

int g_total;
FILE* g_file;
void dummyOutPut(const char *msg , int len)
{
    g_total+=len;
    if(g_file)
    {
        fwrite(msg, 1, len, g_file);
    }
//    else if(g_logFile)
//    {
//        g_logFile
//    }
}

void logInThread()
{
    LOG_INFO<<"logInThread";
    usleep(1000);
}

void bench(const char* type)
{
    Logger::setOutput(dummyOutPut);
}

int main()
{
    Logger::setLogLevel(Logger::DEBUG);// set log out put level
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

    sleep(1);
    bench("nop");
}