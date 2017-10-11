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
         mutex_(threadSafe?new MutexLock:NULL),
         startOfPeriod_(0),
         lastRoll_(0),
         lastFlush_(0)
{
    assert(basename.find('/')==string::npos);
    rollFile();
}