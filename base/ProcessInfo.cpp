//
// Created by root on 17-10-12.
//

#include "ProcessInfo.h"

#include <unistd.h>//getpid()

namespace ProcessInfo
{

    pid_t  pid()
    {
        return ::getpid();
    }


    string hostname()
    {
        char buf[256];
        if(::gethostname(buf, sizeof buf)== 0)
        {
            buf[sizeof(buf)-1] = '\0';
            return buf;
        }
        else
        {
            return "unknownhost";
        }
    }
}