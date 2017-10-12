//
// Created by root on 17-10-11.
//


#include "./LogFile.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

LogFile::LogFile(const string &basename, size_t rollSize, bool threadSafe, int flushInterval, int checkEveryN)
        :basename_(basename),
         rollSize_(rollSize),
         flushInterval_(flushInterval),
         checkEveryN_(checkEveryN),
         count_(0),
         mutex_(threadSafe?new MutexLock:NULL), //不是线程安全就不需要构造mutex_
         startOfPeriod_(0),
         lastRoll_(0),
         lastFlush_(0)
{
    assert(basename.find('/')==string::npos);
    rollFile();
}

LogFile::~LogFile()
{
}

void LogFile::append(const char* logline, int len)
{
    if(mutex_) //如果new过锁了，说明需要线程安全，那么调用加锁方式
    {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    }
    else
    {
        append_unlocked(logline, len); //否则调用非加锁方式
    }
}


void LogFile::flush()
{
    if(mutex_)
    {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}

void LogFile::append_unlocked(const char *logline, int len)
{
    file_->append(logline, len);
    if(file_->writtenBytes()>rollSize_)
    {

    }
    else
    {

    }
}


bool LogFile::rollFile()
{

}

string LogFile::getLogFileName(const string &basename, time_t *now)
{

}