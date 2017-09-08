#ifndef MYTHREAD_LIBRARY_H
#define MYTHREAD_LIBRARY_H
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <string>
#include <exception>

#include "./Atomic.h"

using std::string;
class Thread
{
public:
    typedef boost::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const string& name = string());
    void start();
    int join();
    ~Thread();

    bool started() const { return started_;}
    const string& name() const{ return name_;}
    pid_t tid() const { return *tid_;}
    static int numCreated(){ return numCreated_.get();}

private:
    void setDefaultName();

    ThreadFunc func_;
    pthread_t pthreadId_;
    boost::shared_ptr<pid_t> tid_;
    string name_;

    bool started_;
    bool joined_;

    static AtomicInt32 numCreated_;

};



void hello();

#endif