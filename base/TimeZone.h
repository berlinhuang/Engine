//
// Created by root on 17-9-14.
//

#ifndef ENGINE_TIMEZONE_H
#define ENGINE_TIMEZONE_H
#include <boost/shared_ptr.hpp>

#include <boost/shared_ptr.hpp>

class TimeZone {
public:
    explicit  TimeZone(const char* zonfile);

    TimeZone(int eastOFUtc, const char* tzname);

    TimeZone(){}


    bool valid() const
    {
        return static_cast<bool>(data_);
    }

    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    time_t fromLocalTime(const struct tm&) const;

    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);

    static time_t  fromUtcTime(const struct tm&);

    static time_t fromUtcTime( int year, int month, int day, int hour, int minus, int seconds);

    struct Data;
private:

    boost::shared_ptr<Data> data_;
};


#endif //ENGINE_TIMEZONE_H
