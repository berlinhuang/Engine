//
// Created by root on 17-12-1.
//

#ifndef ENGINE_EVENTLOOPTHREADPOOL_H
#define ENGINE_EVENTLOOPTHREADPOOL_H


#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "./../base/Type.h"



class EventLoop;
class EventLoopThread;



class EventLoopThreadPool {
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* baseLoop, const string& nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads)
    {
        numThreads_ = numThreads;
    }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());


    EventLoop* getNextLoop();

    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();


    bool started() const
    {
        return started_;
    }

    const string& name() const
    {
        return name_;
    }



private:
    EventLoop* baseLoop_;
    string name_;
    bool started_;
    int numThreads_;
    int next_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;

};


#endif //ENGINE_EVENTLOOPTHREADPOOL_H
