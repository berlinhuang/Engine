//
// Created by root on 17-9-4.
//

#ifndef MYTHREAD_TIMESTAMP_H
#define MYTHREAD_TIMESTAMP_H

#include <stdint.h>
#include <time.h>
#include <string>

using std::string;
class Timestamp
{
public:
    Timestamp():microSecondsSinceEpoch_(0){}
    explicit Timestamp(int64_t microSecondsSinceEpochArg):microSecondsSinceEpoch_(microSecondsSinceEpochArg){}

    void swap(Timestamp& that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    string toString() const;
    string toFormattedString( bool showMicroseconds = true) const;

    bool valid()const
    {
        return microSecondsSinceEpoch_>0;
    }
    int64_t microSecondsSinceEpoch() const
    {
        return microSecondsSinceEpoch_;
    }

    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t >(microSecondsSinceEpoch_/kMicroSecondsPersecond);
    }

    static Timestamp now();

    static Timestamp invalid()
    {
        return Timestamp();
    }
    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t,0);
    }
    static Timestamp fromUnixTime(time_t t,int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t)*kMicroSecondsPersecond + microseconds);
    }

    static const int kMicroSecondsPersecond = 1000*1000;
private:
    int64_t microSecondsSinceEpoch_;
};



#endif //MYTHREAD_TIMESTAMP_H
