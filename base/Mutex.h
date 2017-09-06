//
// Created by root on 17-9-4.
//
/**
 *
 *
 * 封装临界区（critical section）
 * 临界区在Windows 上是struct CRITICAL_SECTION，是可重入的；
 * 在Linux 下是pthread_mutex_t，默认是不可重入的
 *
 *
 * 1. raii 资源获取就是初始化  C#的using语句 MutexLock
 * 2. 非递归的mutex（不可重入）
 * 3. 不手工调用lock() unlock() 由Guard对象的构造和析构负责，Guard的生命期就是临界区 scoped locking 区域锁
 *
 *
 * 加锁原语 同步原语 保护了临界区
 *
 */
#ifndef MYTHREAD_MUTEX_H
#define MYTHREAD_MUTEX_H
#include "CurrentThread.h"
#include <assert.h>
#include <pthread.h>


//封装临界区
class MutexLock
{
public:
    MutexLock():holder_(0)
    {
        pthread_mutex_init(&mutex_,NULL);
    }
    ~MutexLock()
    {
        holder_==0;
        pthread_mutex_destroy(&mutex_);
    }
    bool isLockByThisThread() const
    {
        return holder_ == CurrentThread::tid();
    }
    void assertLocked() const
    {
        assert( isLockByThisThread());
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
        assertLocked();
    }
    void unlock()
    {
        unassignHolder();
        pthread_mutex_unlock( &mutex_ );
    }
    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }
private:
    friend class Condition;
    class UnassignGuard
    {
    public:
        UnassignGuard(MutexLock& owner):owner_(owner)
        {
            owner_.unassignHolder();
        }
        ~UnassignGuard()
        {
            owner_.assignHolder();
        }
    private:
        MutexLock& owner_;

    };
    void unassignHolder()
    {
        holder_=0;
    }
    void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }
    pthread_mutex_t mutex_;
    pid_t holder_;
};


//封装临界区的进去和退出 MutexLockGuard一般是栈上对象,作用域刚好等于临界区
class MutexLockGuard
{
public:
    explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};
#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name");


#endif //MYTHREAD_MUTEX_H
