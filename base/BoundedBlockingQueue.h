//
// Created by root on 17-9-7.
//

#ifndef ENGINE_BOUNDEDBLOCKINGQUEUE_H
#define ENGINE_BOUNDEDBLOCKINGQUEUE_H

#include <boost/circular_buffer.hpp>
#include "./Mutex.h"
#include "./Condition.h"
#include <assert.h>

template <typename T>
class BuoundedBlockingQueue
{
public:
    explicit BoundedBlockingQueue(int maxSize)//有界的阻塞队列
    :mutex_(),
     notEmpty_(mutex_),
     notFull_(mutex_),
     queue_(maxSize)
    {

    }

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        while(queue_.full())
        {
            notFull_.wait();
        }
        assert(!queue_.full());
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
        notFull_.notify();
        return front;
    }


    bool empty() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.empty();
    }


    bool full() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.full();
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

    size_t capacity() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.capacity();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    boost::circular_buffer<T> queue_;//使用boost的环形缓冲区
};
#endif //ENGINE_BOUNDEDBLOCKINGQUEUE_H
