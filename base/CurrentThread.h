//
// Created by root on 17-9-1.
//

#ifndef MYTHREAD_CURRENTTHREAD_H
#define MYTHREAD_CURRENTTHREAD_H
#include <stdint.h>
namespace CurrentThread
{
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char* t_threadName;
    void cachedTid();
    inline int tid()
    {
        //使用 __builtin_expect(x,1), likely() ，执行 if 后面的语句 的机会更大
        //使用 __builtin_expect(x,0), unlikely()，执行 else 后面的语句的机会更大
        if (__builtin_expect(t_cachedTid==0,0))//
        {
            cachedTid();
        }
        return t_cachedTid;//更多的情况下是t_cachedTid ~= 0
    }
    bool isMainThread();

    inline const char* name()
    {
        return t_threadName;
    }
}






#endif //MYTHREAD_CURRENTTHREAD_H
