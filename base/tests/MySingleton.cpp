//
// Created by root on 17-9-7.
//

#include "./../Singleton.h"
#include "./../CurrentThread.h"
#include "./../Thread.h"
#include <string>
#include <boost/bind.hpp>
#include <iostream>
using namespace std;


class Test
{
public:
    Test()
    {
        printf("tid=%d, constructing  %p\n",CurrentThread::tid(),this);

    }
    ~Test()
    {
        printf("tid=%d, constructing  %p %s\n",CurrentThread::tid(),this, name_.c_str());
    }
    const string& name() const { return name_;}
    void setName(const string& n){ name_ = n;}
private:
    string name_;

};



class TestNoDestroy
{
public:
    void no_destroy();
    TestNoDestroy()
    {
        printf("tid=%d, constructing TestNoDestroy %p\n",CurrentThread::tid(),this);
    }
    ~TestNoDestroy()
    {
        printf("tid=%d, destructing TestNoDestroy %p\n",CurrentThread::tid(),this);
    }

};



void threadFunc(string thread)
{
    cout<<thread<<" tid = "<<CurrentThread::tid()<<" "<<&Singleton<Test>::instance()<<" name = "<<Singleton<Test>::instance().name().c_str()<<endl;
    Singleton<Test>::instance().setName("only one, changed");
}



int main()
{
    Singleton<Test>::instance().setName("only one");
    Thread t1(boost::bind(threadFunc,"t1 thread"));
    t1.start();
    t1.join();

    cout<<"main thread tid = "<<CurrentThread::tid()<<" "<<&Singleton<Test>::instance()<<" name = "<<Singleton<Test>::instance().name().c_str()<<endl;

    Singleton<TestNoDestroy>::instance();

    printf("with valgrind, you should see %zd-byte memory leak.\n",sizeof(TestNoDestroy));
}