//
// Created by root on 17-9-8.
//

#include "./../Atomic.h"
#include <assert.h>
int main()
{
    AtomicInt64 a0;
    assert(a0.get()==0);
    assert(a0.getAndAdd(2) == 0);
    assert(a0.get()==2);
    assert(a0.addAndGet(2) == 4);
    assert(a0.get()==4);
    assert(a0.incrementAndGet()==5);
    assert(a0.get()==5);
    a0.increment();
    assert(a0.get()==6);
    assert(a0.addAndGet(-3)==3);
    assert(a0.getAndSet(100)==3);
    assert(a0.get()==100);

}