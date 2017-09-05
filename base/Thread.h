#ifndef MYTHREAD_LIBRARY_H
#define MYTHREAD_LIBRARY_H
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <string>
#include <exception>
using std::string;
class Thread
{
public:
    typedef boost::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const string& name = string());
    void setDefaultName();
    void start();
    int join();
    virtual ~Thread();
private:
    ThreadFunc func_;
    pthread_t pthreadId_;
    boost::shared_ptr<pid_t> tid_;
    string name_;

    bool started_;
    bool joined_;

};



void hello();

#endif