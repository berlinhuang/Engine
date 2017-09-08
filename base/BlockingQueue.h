//
// Created by root on 17-9-7.
//
/*
 *
 * IO MODE(within one thread):
 *
 * blocking
 * non-blocking       (bad:busy-polling to check whether non-blocking IO op is finished)
 * IO multiplexing    (always usage: non-blocking + IO multiplexing)
 * signal-driven
 * asynchronous
 *
 *
 * C++ 标准库里的大多数class 都不是线程安全的，包括std::string、std::vector、std::map 等，
 * 因为这些class 通常需要在外部加锁才能供多个线程同时访问
 */
#ifndef ENGINE_BLOCKINGQUEUE_H
#define ENGINE_BLOCKINGQUEUE_H

#include "./Mutex.h"
#include "./Condition.h"
#include <deque>

template<typename T>
class BlockingQueue
{
public:
    BlockingQueue():mutex_(), notEmpty_(mutex_), queue_()
    {
    }
    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while(queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(queue_.front());

        queue_.pop_front();
        return front;
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

private:
    mutable  MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T> queue_;
};

#endif //ENGINE_BLOCKINGQUEUE_H
