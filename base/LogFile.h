//
// Created by root on 17-10-11.
//

#ifndef ENGINE_LOGFILE_H
#define ENGINE_LOGFILE_H
#include "./Mutex.h"
#include "./FileUtil.h"
#include <boost/scoped_ptr.hpp>
#include <string>
using std::string;

namespace FileUtil
{
    class AppendFile;
}


class LogFile {
public:
    LogFile(const string& basename,
            size_t rollSize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEveryN = 1024);

    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char*  logline, int len);
    static string getLogFileName(const string& basename, time_t* now);
    const string basename_;//即最后/符号后的字符串
    const size_t rollSize_;//日志文件达到rolsize生成一个新文件
    const int flushInterval_;//日志写入间隔时间
    const int checkEveryN_;

    int count_;

    boost::scoped_ptr<MutexLock> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    boost::scoped_ptr<FileUtil::AppendFile> file_;//文件智能指针
    const static int kRollPerSeconds_ = 60* 60*24;
};

















#endif //ENGINE_LOGFILE_H
