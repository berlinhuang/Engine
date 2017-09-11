//
// Created by root on 17-9-9.
//
#include "./../LogStream.h"
#include "./../Timestamp.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

const size_t N = 1000000;

template <typename  T>
void benchPrintf(const char* fmt)
{
    char buf[32];
    Timestamp start( Timestamp::now());
    for(size_t i = 0;i<N;++i)
    {
        snprintf(buf,sizeof buf, fmt,(T)(i));//snprintf(buf,sizeof buf, %d, (int)(i) )
    }
    Timestamp end(Timestamp::now());
    std::cout<<"benchPrintf = "<<timeDifference(end,start)<<std::endl;
}


template<typename T>
void benchStringStream()
{
    Timestamp start(Timestamp::now());
    std::ostringstream os;

    for (size_t i = 0; i < N; ++i)
    {
        os << (T)(i);
        os.seekp(0, std::ios_base::beg);
    }
    Timestamp end(Timestamp::now());

    printf("benchStringStream %f\n", timeDifference(end, start));
}



template <typename T>
void benchLogStream()
{
    Timestamp start(Timestamp::now());
    LogStream os;
    for(size_t i = 0; i<N;++i)
    {
        os<<(T)(i);
        os.resetBuffer();
    }
    Timestamp end(Timestamp::now());
    printf("benchLogStream %f\n",timeDifference(end,start));
}



int main()
{

    benchPrintf<int>("%d");
    puts("int");
    benchPrintf<int>("%d");
    benchStringStream<int>();
    benchLogStream<int>();








}