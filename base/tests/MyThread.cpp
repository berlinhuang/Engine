//
// Created by root on 17-9-1.
//

#include <stdio.h>
#include <boost/bind.hpp>
#include "../Thread.h"
#include "../CurrentThread.h"

void threadFunc()
{
    printf("tid=%d\n", CurrentThread::tid());
}


void threadFuncPara(int x)
{
    printf("tid=%d, x=%d\n",CurrentThread::tid(),x);

}
int main()
{
    printf("main thread pid=%d, tid=%d\n",getpid(),CurrentThread::tid());

    Thread t1(threadFunc);
    t1.start();
    t1.join();


    Thread t2(boost::bind(threadFuncPara,55),"thread for free func with argument");
    t2.start();
    t2.join();


    return 0;
}