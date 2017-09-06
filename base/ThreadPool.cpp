//
// Created by root on 17-9-5.
//

#include "ThreadPool.h"
#include <boost/bind.hpp>
#include "Exception.h"

ThreadPool::ThreadPool(const string& nameArg)
:mutex_(),
 notEmpty_(mutex_),//    MutexLock& mutex_;  pthread_cond_dt pcond_;
 notFull_(mutex_),
 name_(nameArg),
 maxQueueSize_(0),
 running_(false)
{

}


ThreadPool::~ThreadPool()
{
    if(running_)
    {
        stop();
    }
}


void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
    }
    for_each(threads_.begin(),threads_.end(),boost::bind(&Thread::join,_1));
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for(int i = 0;i<numThreads;++i)
    {
        char id[32];
        snprintf(id,sizeof id,"%d",i+1);
        threads_.push_back(new Thread(boost::bind(&ThreadPool::runInThread,this),name_+id));
        threads_[i].start();
    }
    if(numThreads==0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

/*
 *
 *   run()-> push  ---------任务队列----------  front->take()
 *
 *
 */


void ThreadPool::run(const Task& task)
{
    if(threads_.empty())
    {
        task();//run task
    }
    else//
    {
        MutexLockGuard lock(mutex_);
        while(isFull())//当任务队列满的时候，循环
        {
            notFull_.wait(); //一直等待任务队列不满
        }
        assert(!isFull());
        queue_.push_back(task);
        notEmpty_.notify();
    }
}

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);
    while(queue_.empty() && running_ )
    {
        notEmpty_.wait();
    }
    Task task;
    if(!queue_.empty())
    {
        task=queue_.front();
        queue_.pop_front();
    }
    if(maxQueueSize_>0)
    {
        notFull_.notify();
    }
    return task;
}

bool ThreadPool::isFull() const
{
    mutex_.assertLocked();
    return maxQueueSize_>0 && queue_.size()>=maxQueueSize_;

}



void ThreadPool::runInThread()
{
    try
    {
        if(threadInitCallback_)
        {
            threadInitCallback_();
        }
        while(running_)
        {
            Task task(take());
            if(task)
            {
                task();
            }
        }

    }
    catch( const Exception& ex)
    {
        fprintf(stderr,"exception caught in ThreadPool %s\n",name_.c_str());
        fprintf(stderr,"reason:%s\n",ex.what());
        fprintf(stderr,"stack trace:%s\n",ex.stackTrace());
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr,"exception caught in ThreadPool %s\n",name_.c_str());
        fprintf(stderr,"reason:%s\n",ex.what());
        abort();
    }
    catch(...)
    {
        fprintf(stderr,"unknown exception caught in ThreadPool %s\n",name_.c_str());
        throw;
    }
}











