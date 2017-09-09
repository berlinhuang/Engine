//
// Created by root on 17-9-9.
//
#include "./../LogStream.h"
#include "./../Timestamp.h"
#include <iostream>

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













int main()
{

benchPrintf<int>("%d");











}