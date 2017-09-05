//
// Created by root on 17-9-4.
//
#include <string>
#include "Timestamp.h"
#include <inttypes.h>
#include <sys/time.h>
#include <stdio.h>


using std::string;

string Timestamp::toString() const
{
    char buf[32] ={0};
    int64_t  seconds = microSecondsSinceEpoch_/kMicroSecondsPersecond;
    int64_t  microseconds = microSecondsSinceEpoch_%kMicroSecondsPersecond;
    snprintf(buf,sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "",seconds, microseconds);
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds*kMicroSecondsPersecond+tv.tv_usec);
}