//
// Created by root on 17-9-8.
//

//多线程下变量-原子操作

#ifndef ENGINE_ATOMIC_H
#define ENGINE_ATOMIC_H

#include <stdint.h>

template <typename T>
class AtomicIntergerT
{
public:
    AtomicIntergerT()
            :value_(0)
    {}

    T get()
    {
        return __sync_val_compare_and_swap(&value_,0,0);
    }
    T getAndAdd(T x)
    {
        return __sync_fetch_and_add(&value_,x);
    }

    T addAndGet(T x)
    {
        return getAndAdd(x)+x;
    }

    T incrementAndGet()
    {
        return addAndGet(1);
    }

    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    void add(T x)
    {
        getAndAdd(x);
    }

    void increment()
    {
        incrementAndGet();
    }

    void decrement()
    {
        decrementAndGet();
    }

    T getAndSet(T newValue)
    {
        return __sync_lock_test_and_set(&value_,newValue);
    }

private:
    volatile T value_;

};


typedef AtomicIntergerT<int32_t> AtomicInt32; //32bit
typedef AtomicIntergerT<int64_t> AtomicInt64; //64bit


#endif //ENGINE_ATOMIC_H
