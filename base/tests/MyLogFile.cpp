//
// Created by root on 17-10-11.
//

#include "./../LogFile.h"
#include "./../Logging.h"

boost::scoped_ptr<LogFile> g_logFile;//全局变量，输出到这里

void outputFunc(const char* msg, int len)
{
    g_logFile->append(msg, len);
    // scoped_ptr<T> 重载operator->，
    // 调用LogFile::append(),
    // 间接调用File::append();
    // 最后 ::fwrite_unlocked(fp_);
}

void flushFunc()
{
    g_logFile->flush();
    // scoped_ptr<T> 重载operator->，
    // 调用LogFile::flush(),
    // 间接调用File::flush()，
    // 最后::fflush(fp_);
}


int main(int argc, char* argv[])
{
    char name[256];
    strncpy(name, argv[0],256);
    g_logFile.reset(new LogFile(::basename(name),200*1000));//删除原来保存的指针，再保存新的指针new LogFile，如果new LogFile是空指针，那scoped_ptr将不持有任何指针
    Logger::setOutPut(outputFunc);
    Logger::setFlush(flushFunc);
    string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    for(int i=0;i<100;++i)
    {
        // ./Engine/cmake-build-debug/base/tests/CMakeFiles/MylogFile.20171013-020448.berlin-virtual-machine.16353.log
        LOG_INFO<<line<<i;
        // 不断地构造匿名的Logger对象，在~Logger()中调用dummyOutput，将日志信息写入文件
        usleep(1000);
    }

}















































