//
// Created by root on 17-10-12.
//

#include "ProcessInfo.h"
#include "FileUtil.h"
#include <unistd.h>//getpid()
#include <dirent.h>
#include <assert.h>
#include <algorithm>
#include <sys/resource.h>// getrlimit
#include <sys/times.h>
#include <pwd.h>


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

    Timestamp g_startTime = Timestamp::now();
// assume those won't change during the life time of a process.
    int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
    int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));

}

using namespace detail;

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

    int clockTicksPerSecond()
    {
        return g_clockTicks;
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

    int openedFiles()
    {
        t_numOpenedFiles = 0;
        scanDir("/proc/self/fd",fdDirFilter);
        return t_numOpenedFiles;
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


    int maxOpenFiles()
    {
        struct rlimit rl;
        if (::getrlimit(RLIMIT_NOFILE, &rl))
        {
            return openedFiles();
        }
        else
        {
            return static_cast<int>(rl.rlim_cur);
        }
    }

    CpuTime cpuTime()
    {
        ProcessInfo::CpuTime t;
        struct tms tms;
        if (::times(&tms) >= 0)
        {
            const double hz = static_cast<double>(clockTicksPerSecond());
            t.userSeconds = static_cast<double>(tms.tms_utime) / hz;
            t.systemSeconds = static_cast<double>(tms.tms_stime) / hz;
        }
        return t;
    }

    string exePath()
    {
        string result;
        char buf[1024];
        ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
        if (n > 0)
        {
            result.assign(buf, n);
        }
        return result;
    }

    bool isDebugBuild()
    {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

    StringPiece procname(const string& stat)
    {
        StringPiece name;
        size_t lp = stat.find('(');
        size_t rp = stat.rfind(')');
        if (lp != string::npos && rp != string::npos && lp < rp)
        {
            name.set(stat.data()+lp+1, static_cast<int>(rp-lp-1));
        }
        return name;
    }

    string procname()
    {
        return procname(procStat()).as_string();
    }


    string procStatus()
    {
        string result;
        FileUtil::readFile("/proc/self/status", 65536, &result);
        return result;
    }

    string procStat()
    {
        string result;
        FileUtil::readFile("/proc/self/stat", 65536, &result);
        return result;
    }

    string username()
    {
        struct passwd pwd;
        struct passwd* result = NULL;
        char buf[8192];
        const char* name = "unknownuser";

        getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
        if (result)
        {
            name = pwd.pw_name;
        }
        return name;
    }
}