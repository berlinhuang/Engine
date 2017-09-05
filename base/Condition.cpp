//
// Created by root on 17-9-5.
//

#include "Condition.h"
#include <errno.h>

bool Condition::waitForSeconds(double seconds)
{
    struct timespec abstime;

    clock_gettime(CLOCK_REALTIME, &abstime);

    const int64_t kNanoSecondsPerSecond = 1e9;

    int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec+nanoseconds)/kNanoSecondsPerSecond);
    abstime.tv_nsec = static_cast<long>((abstime.tv_nsec+nanoseconds)%kNanoSecondsPerSecond);

    MutexLock::UnassignGuard ug(mutex_);

    return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(),&abstime);
    //pthread_mutex_unlock
    //等待唤醒
    //唤醒后第一件事情是上锁相当于pthread_mutex_lock
}


















