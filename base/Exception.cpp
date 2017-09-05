//
// Created by root on 17-9-2.
//

#include "Exception.h"
#include "execinfo.h"



Exception::Exception(const char* msg):message_(msg)
{
    fillStackTrace();
}


Exception::Exception( const string& msg):message_(msg)
{
    fillStackTrace();
}

const char* Exception::what() const throw()//对使用throw()修饰的函数进行try{}catch(...){}是没有任何意义的
{
    return message_.c_str();
}

const char* Exception::stackTrace() const throw()
{
    return stack_.c_str();
}

Exception::~Exception() throw()
{

}

void Exception::fillStackTrace()
{
    const int len = 200;
    void* buffer[len];
    int nptrs = ::backtrace(buffer,len);
    char **strings = ::backtrace_symbols(buffer,nptrs);
    if(strings)
    {
        for(int i=0;i<nptrs;++i)
        {
            stack_.append(strings[i]);
            stack_.push_back('\n');
        }
        free(strings);
    }

}