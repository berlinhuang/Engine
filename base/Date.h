//
// Created by root on 17-9-6.
//

#ifndef ENGINE_DATE_H
#define ENGINE_DATE_H
#include <string>
using std::string;
class Date
{
public:
    struct YearMonthDay
    {
        int year;
        int month;
        int day;
    };
    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    Date():julianDayNumber_(0){}

    Date(int year, int month, int day);

    explicit Date(int julianDayNum):julianDayNumber_(julianDayNum){}

    explicit Date(const struct tm&);

    void swap(Date& that)
    {
        std::swap(julianDayNumber_, that.julianDayNumber_);
    }

    bool valid() const { return julianDayNumber_> 0;}

    string toIsoString() const;

    struct YearMonthDay yearMonthDay() const;

    int year() const
    {
        return YearMonthDay().year;
    }

    int month() const
    {
        return YearMonthDay().month;
    }

    int day() const
    {
        return YearMonthDay().day;
    }
    // [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
    int weekDay() const
    {
        return (julianDayNumber_+1)%kDaysPerWeek;
    }

    int julianDayNumber() const { return julianDayNumber_; }

private:
    int julianDayNumber_;




};



#endif //ENGINE_DATE_H
