//
// Created by root on 17-10-11.
//

#include "./../FileUtil.h"
//#include <string>
//using std::string;
#include <inttypes.h>
#include <stdio.h>

int main()
{
    string result;
    int64_t size = 0;
    int err = FileUtil::readFile("/proc/self", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/proc/self", 1024, &result, NULL);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/proc/self/cmdline", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/null", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/zero", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/notexist", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/zero", 102400, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/zero", 102400, &result, NULL);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
}