//
// Created by root on 17-9-5.
//

/*
 *
 *
 *
 */

#include "CountDownLatch.h"
CountDownLatch::CountDownLatch(int count)
:mutex_(),
 condition_(mutex_),
 count_(count)

{

}


void CountDownLatch::wait()
{
    MutexLockGuard lock(mutex_);
    while(count_>0)
    {
        condition_.wait();
    }
}

void CountDownLatch::countDown()
{
    MutexLockGuard lock(mutex_);
    --count_;
    if(count_==0)
    {
        condition_.notifyAll();
    }
}


int CountDownLatch::getCount() const
{
    MutexLockGuard lock(mutex_);// lock 的析构会晚于返回对象的构造
    return count_;
    // lock会在这里析构,作用域结束的时候,因此可以有效包括这个功效数据

}

