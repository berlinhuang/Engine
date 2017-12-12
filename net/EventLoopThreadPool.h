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
    int numThreads_; // 线程数
    int next_; // 新连接到来，所选择的EventLoop对象下标
    //std::vector<boost::shared_ptr<EventLoopThread>> threads_;//效率不高
    boost::ptr_vector<EventLoopThread> threads_;//指针容器 IO线程列表
    std::vector<EventLoop*> loops_;// EventLoop列表

};


#endif //ENGINE_EVENTLOOPTHREADPOOL_H
