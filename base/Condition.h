//
// Created by root on 17-9-5.
//

#ifndef ENGINE_CONDITION_H
#define ENGINE_CONDITION_H
#include "./Mutex.h"
#include <pthread.h>
class Condition
{
public:
    explicit Condition( MutexLock& mutex):mutex_(mutex)
    {
        pthread_cond_init(&pcond_,NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }
    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_);//mutex_.unassignHolder();// holder_=0;
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }
    bool waitForSeconds(double seconds);
    void notify()
    {
        pthread_cond_signal(&pcond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }
private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};



#endif //ENGINE_CONDITION_H
