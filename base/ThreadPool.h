//
// Created by root on 17-9-5.
//

#ifndef ENGINE_THREADPOOL_H
#define ENGINE_THREADPOOL_H

#include <deque>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>
#include "Thread.h"
#include "Mutex.h"
#include "Condition.h"
class ThreadPool
{
public:
    typedef boost::function<void()> Task;
    explicit ThreadPool(const string& nameArg);
    ~ThreadPool();


    void setMaxQueueSize(int maxSize){ maxQueueSize_ = maxSize; }
    void setThreadInitCallback(const Task& cb){ threadInitCallback_ = cb; }

    void start(int numThreads);
    void stop();

    const string& name() const { return name_;}
    size_t queueSize() const;

    void run(const Task& f);

private:
    bool isFull const;
    void runInThread();
    Task take();



    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    string name_;
    Task threadInitCallback_;
    boost::ptr_vector<Thread> threads_;
    std::deque<Task> queue_;
    size_t maxQueueSize_;
    bool running_;


};




#endif //ENGINE_THREADPOOL_H
