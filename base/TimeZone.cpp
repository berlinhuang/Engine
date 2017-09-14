//
// Created by root on 17-9-14.
//

#include "TimeZone.h"
#include <vector>


struct Transition
{
    time_t gmtime;
    time_t localtime;
    int localtimeIdx;
    Transition(time_t t, time_t l, int localIdx):gmtime(t), localtime(l),localtimeIdx(localIdx)
    {
    }
};

struct Comp
{
    bool compareGmt;
    Comp(bool gmt):compareGmt(gmt)
    {
    }

    bool operator()(const Transition& lhs, const Transition& rhs) const
    {
        if(compareGmt)
            return lhs.gmtime<rhs.gmtime;
        else
            return lhs.localtime == rhs.localtime;
    }
};


struct Localtime
{

};

using namespace std;
struct TimeZone::Data
{
    vector<Transition> transition;
    vector<Localtime> localtimes;
    vector<string> names;
    string abbreviation;
};


class File
{
public:
    File(const char* file):fp_(::fopen(file, "rb"))
    {

    }
    ~File()
    {
        if(fp_)
        {
            ::fclose(fp_);
        }
    }
    bool valid() const { return fp_;}






private:
    FILE* fp_;

};

/////////////////////////////////////////////////////////////////////////////////////////
bool readTimeZoneFile(const char* zonefile, struct TimeZone::Data* data)
{


}

TimeZone::TimeZone(const char *zonefile):data_(new TimeZone::Data)
{
    if(readTimeZoneFile(zonefile, data_.get()))
    {
        data_.reset();
    }

}


struct tm TimeZone::toLocalTime(time_t seconds) const
{
    struct tm localTime;



    return localTime;
}



