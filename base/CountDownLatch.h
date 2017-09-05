//
// Created by root on 17-9-5.
//

#ifndef ENGINE_COUNTDOWNLATCH_H
#define ENGINE_COUNTDOWNLATCH_H

#include "./Mutex.h"
#include "./Condition.h"

class CountDownLatch
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    //被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中
    mutable  MutexLock mutex_;//mutable 意味着const CountDownLatch::getCount() 也能直接使用non-const 的mutex_
    Condition condition_;
    int count_;
};



#endif //ENGINE_COUNTDOWNLATCH_H
