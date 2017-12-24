//
// Created by root on 17-10-12.
//

#ifndef ENGINE_PROCESSINFO_H
#define ENGINE_PROCESSINFO_H

#include "StringPiece.h"
#include "Timestamp.h"
#include <vector>



namespace  ProcessInfo
{
    pid_t  pid();


    string pidString();
    uid_t uid();
    string username();
    uid_t euid();

    Timestamp startTime();
    int clockTicksPerSecond();
    int pageSize();
    bool isDebugBuild();

    string hostname();
    string procname();

    StringPiece procname(const string& stat);

    string procStatus();
    string procStat();
    string threadStat();
    string exePath();

    int openedFiles();
    int maxOpenFiles();


    struct CpuTime
    {
        double userSeconds;
        double systemSeconds;
        CpuTime():userSeconds(0.0),systemSeconds(0.0){}
    };
    CpuTime cpuTime();
    int numThreads();
    std::vector<pid_t> threads();

}

#endif //ENGINE_PROCESSINFO_H
