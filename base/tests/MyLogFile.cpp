//
// Created by root on 17-10-11.
//

#include "./../LogFile.h"
#include "./../Logging.h"

boost::scoped_ptr<LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
    g_logFile->append(msg, len);
}

void flushFunc()
{
    g_logFile->flush();
}


int main(int argc, char* argv[])
{
    char name[256];
    strncpy(name, argv[0],256);
    g_logFile.reset(new LogFile(::basename(name),200*1000));
    Logger::setOutPut(outputFunc);
    Logger::setFlush(flushFunc);
    string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    for(int i=0;i<10000;++i)
    {
        LOG_INFO<<line<<i;
        usleep(1000);
    }

}















































