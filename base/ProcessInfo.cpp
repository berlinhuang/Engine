//
// Created by root on 17-10-12.
//

#include "ProcessInfo.h"
#include "FileUtil.h"
#include <unistd.h>//getpid()
#include <dirent.h>
#include <assert.h>
#include <algorithm>

namespace  detail
{
    __thread int t_numOpenedFiles = 0;
    __thread std::vector<pid_t>* t_pids = NULL;
    int fdDirFilter(const struct dirent* d )
    {
        if(::isdigit(d->d_name[0]))
        {
          ++t_numOpenedFiles;
        }
        return 0;
    }

    int scanDir(const char *dirpath, int (*filter)(const struct dirent*))
    {
        struct dirent** namelist = NULL;
        int result = ::scandir(dirpath, &namelist, filter, alphasort);
        assert(namelist == NULL);
        return result;
    }

    int taskDirFilter(const struct dirent* d)
    {
        if(::isdigit(d->d_name[0]))
        {
            t_pids->push_back(atoi(d->d_name));
        }
        return 0;
    }

}

Timestamp g_startTime = Timestamp::now();



namespace ProcessInfo
{

    pid_t  pid()
    {
        return ::getpid();
    }

    uid_t uid()
    {
        return ::getuid();
    }

    uid_t euid()
    {
        return ::geteuid();
    }

    Timestamp startTime()
    {
        return g_startTime;
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

    using namespace detail;
    int openedFiles()
    {
        t_numOpenedFiles = 0;
        scanDir("/proc/self/fd",fdDirFilter);
        return t_numOpenedFiles;
    }

    string procStatus()
    {
        string result;
        FileUtil::readFile("/proc/self/status", 65536, &result);
        return result;
    }

    int numThreads()
    {
        int result = 0;
        string status = procStatus();
        size_t pos = status.find("Threads:");
        if (pos != string::npos)
        {
            result = ::atoi(status.c_str() + pos + 8);
        }
        return result;
    }

    std::vector<pid_t> threads()
    {
        std::vector<pid_t> result;
        t_pids = &result;
        scanDir("/proc/self/task", taskDirFilter);
        t_pids = NULL;
        std::sort(result.begin(), result.end());
        return result;
    }


}