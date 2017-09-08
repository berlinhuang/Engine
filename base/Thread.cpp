#include "Thread.h"

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <syscall.h>
#include "CurrentThread.h"
#include "Exception.h"
#include <stdlib.h>
/*
 * process pid_t        diff
 * thread  pthread_t    may be same
 * thread  tid          diff       syscall(SYS_gettid)
 * */

namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char * t_threadName = "unknown";
    const bool sameType = boost::is_same<int, pid_t>::value;


    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }
    void cachedTid()
    {
        if(t_cachedTid == 0)
        {
            t_cachedTid = gettid();
            t_tidStringLength = snprintf(t_tidString,sizeof t_tidString, "%5d ",t_cachedTid);
        }
    }
    bool isMainThread()
    {
        return tid() == ::getpid();// 主线程的线程id和进程id相同
    }



}




struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    boost::weak_ptr<pid_t> wkTid_;


    ThreadData(const ThreadFunc& func, const string& name, const boost::shared_ptr<pid_t>& tid)
    :func_(func),name_(name),wkTid_(tid)
    {}

    void runInThread()
    {
        boost::shared_ptr<pid_t> ptid = wkTid_.lock();
        if(ptid)
        {
            //*ptid = tid;
            ptid.reset();
        }
        try {
            func_();
            CurrentThread::t_threadName="finished";
        }
        catch (const Exception& ex)
        {
            CurrentThread::t_threadName="crashed";
            fprintf(stderr,"exception caught in thread %s\n", name_.c_str());
            fprintf(stderr,"reason:%s\n",ex.what());
            abort();
        }
        catch (const std::exception& ex)
        {

            CurrentThread::t_threadName="crashed";
            fprintf(stderr,"exception caught in thread %s\n",name_.c_str());
            fprintf(stderr,"reason:%s\n",ex.what());
        }
        catch(...)
        {
            CurrentThread::t_threadName="crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw;
        }

    }
};


void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if(name_.empty())
    {
        char buf[32];
        snprintf(buf,sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}


AtomicInt32 Thread::numCreated_;


Thread::Thread( const ThreadFunc& func, const string & n )
:pthreadId_(0),
 func_(func),
 name_(n),
 started_(false),
 joined_(false)
{
    setDefaultName();
}

void *startThread(void*obj)
{
    ThreadData* data= static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

void Thread::start()
{
    assert(!started_);
    started_=true;
    ThreadData* data = new ThreadData( func_, name_, tid_);
    if(pthread_create(&pthreadId_, NULL, startThread, data))
    {
        started_=false;
    }
}

int  Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_=true;
    return pthread_join(pthreadId_,NULL);

}

Thread::~Thread()
{
    if(started_ && !joined_ )
    {
        pthread_detach(pthreadId_);
    }
}

void hello() {
    std::cout << "Hello, World!" << std::endl;
}