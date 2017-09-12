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


// istream& seekg ( streampos pos );
// istream& seekg ( streamoff off, ios_base::seekdir dir ); 是用来调整输入流文件指针的，其中ios_base::seekdir 包括

// ios_base::beg	 beginning of the stream buffer
// ios_base::cur	 current position in the stream buffer
// ios_base::end	 end of the stream buffer
// file_in.seekg(12,ios_base::cur); 将文件指针定位至当前位置往后12个字节的地方
template<typename T>
void benchStringStream()
{
    Timestamp start(Timestamp::now());
    std::ostringstream os;

    for (size_t i = 0; i < N; ++i)
    {
        os << (T)(i);
        os.seekp(0, std::ios_base::beg);// beginning of the stream buffer 将文件指针定位至文件开始位置
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