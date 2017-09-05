//
// Created by root on 17-9-4.
//
#include "../Mutex.h"
#include <vector>
#include <stdlib.h>
MutexLock g_mutex;
std::vector<int> g_vec;
const int kCount = 10*1000*1000
int g_count = 0;
int foo()
{
    MutexLockGuard lock(g_mutex);
    if(!g_mutex.isLockByThisThread())
    {
        printf("fail\n");
        return -1;
    }
    ++g_count;
    return 0;
}
int main()
{
    foo();
    if(g_count!=1)
    {
        printf("calls twice.\n");
        abort();
    }

    const int kMaxThreads = 8;
    g_vec.reserve(kMaxThreads * kCount);

}