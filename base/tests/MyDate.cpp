//
// Created by root on 17-9-6.
//

#include "./../Date.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include <iostream>
/*
 *
 * struct tm
 * {
 * int tm_sec;            Seconds. [0-60] (1 leap second)
 * int tm_min;           Minutes. [0-59]
 * int tm_hour;          Hours.   [0-23]
 * int tm_mday;          Day.     [1-31]
 * int tm_mon;           Month.   [0-11]
 * int tm_year;          Year - 1900.
 * int tm_wday;          Day of week. [0-6]
 * int tm_yday;          Days in year.[0-365]
 * int tm_isdst;         DST.     [-1/0/1]
 * #ifdef  __USE_BSD
 *      long int tm_gmtoff;        Seconds east of UTC.
 *      __const char *tm_zone;     Timezone abbreviation.
 * #else
 *      long int __tm_gmtoff;      Seconds east of UTC.
 *      __const char *__tm_zone;   Timezone abbreviation.
 * #endif
 * };
 * 这两个函数的原型为：
 *
 * struct tm *localtime(const time_t *timep);
 * struct tm *gmtime(const time_t *timep);
 *
 */
const int kMonthsOfYear = 12;


void passByConstRefer(const Date& x)
{
    printf("%s\n",x.toIsoString().c_str());
}

void passByValue( Date x)
{
    printf("%s\n",x.toIsoString().c_str());
}


int isLeapYear(int year)
{
    if (year % 400 == 0)
        return 1;
    else if (year % 100 == 0)
        return 0;
    else if (year % 4 == 0)
        return 1;
    else
        return 0;
}


int daysOfMonth(int year, int month)
{
    static int days[2][kMonthsOfYear+1] =
            {
                    { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
                    { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
            };
    return days[isLeapYear(year)][month];
}


int main()
{
    time_t now = time(NULL);
    //get current time
    struct tm t1 = *gmtime(&now);
    struct tm t2 = *localtime(&now);

    Date someDay(2008,9,10);

    printf("%s\n",someDay.toIsoString().c_str());

    passByValue(someDay);

    passByConstRefer(someDay);

    Date todayUtc(t1);
    printf("%s\n", todayUtc.toIsoString().c_str());

    Date todayLocal(t2);
    printf("%s\n", todayLocal.toIsoString().c_str());


    int julianDayNumber = 2415021;
    int weekDay = 1;
    for(int year =1900;year<2500;++year)
    {
        assert(Date(year, 3, 1).julianDayNumber() - Date(year, 2, 29).julianDayNumber()
               == isLeapYear(year));
        std::cout<<Date(year, 3, 1).julianDayNumber()<<" "<<Date(year, 2, 29).julianDayNumber()<<" "<<Date(year, 3, 1).julianDayNumber() - Date(year, 2, 29).julianDayNumber()<<std::endl;

        for(int month = 1;month <= kMonthsOfYear;++month)
        {
            for(int day = 1;day<= daysOfMonth(year,month);++day)
            {
                Date d(year, month,day);

                Date d2(julianDayNumber);

                ++julianDayNumber;
                weekDay - (weekDay+1)%7;
            }
        }

    }
    printf("all passed.\n");
}