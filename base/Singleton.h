//
// Created by root on 17-9-7.
//

/*
 * thread safe singleton
 *
 */
#ifndef ENGINE_SINGLETON_H
#define ENGINE_SINGLETON_H
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
/**
如果是C++0x，则
#define __cplusplus 1
#define __GXX_EXPERIMENTAL_CXX0X__ 1

如果是C++11，则
#define __cplusplus 201103L
#define __GXX_EXPERIMENTAL_CXX0X__ 1

如果是C++14，则
#define __cplusplus 201402L
#define __GXX_EXPERIMENTAL_CXX0X__ 1
所以，要检测是否c++11，则一般是#if __cplusplus >= 201103L
如果失败再尝试使用GXX_EXPERIMENTAL_CXX0X来检测。

 **/


template<typename T>
// 是在检测 T::no_destroy 是否存在。 value = true when has T::no_destry
struct has_no_destroy
{
    //第1个重载函数
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    template <typename C> static char test(decltype(&C::no_destroy));
#else
    template <typename C> static char test(typeof(&C::no_destroy));
#endif
    //第2个重载函数
    template <typename C> static int32_t test(...);

    //sizeof(test<T>(0)) sizeof()一个函数, 返回的是函数返回值的大小 不需要函数定义，因为实际上并没有真正调用该函数
    const static bool value = sizeof(test<T>(0)) == 1;//判断如果是类的话，是否有no_destroy方法    sizeof(char) = 1  const static bool value = sizeof(test<T>(nullptr))== sizeof(char)
};


template <typename T>
class Singleton
{
public:
    static T& instance()
    {
        pthread_once(&ponce_,&Singleton::init); //多个线程只会执行一次init函数
        return *value_;
    }


private:
    Singleton();
    ~Singleton();

    static void init()
    {
        value_ = new T();
        if(!has_no_destroy<T>::value) //1.当参数是类且没有"no_destroy"方法才会注册atexit的destroy
        {

            //std::cout<<"value..........: "<< has_no_destroy<T>::value<<std::endl; //have destroy func 0
            //登记atexit时调用的销毁函数，防止内存泄漏
            ::atexit(destroy); //在程序结束的时候会自动调用销毁函数，就不用手动调用了
        }
        //2.检测模板参数T如果是类的话，并且该类注册了一个no_destroy()方法，那么muduo库不会去自动销毁它

    }

    static void destroy()
    {
        /*
         * 在C++中，类型有Complete type和Incomplete type之分，
         * 对于Complete type, 它的大小在编译时是可以确定的，
         * 而对于Incomplete type, 它的大小在编译时是不能确定的
         *
         * 用delete删除一个只有声明但无定义的类型的指针(即不完整类型)，是危险的。
         * 这通常导致无法调用析构函数（包括对象本身的析构函数、成员/基类的析构函数），从而泄露资源
         *
         * 而通过 typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
         * 这种做法能使当T为不完整类型时编译报错。
         * (当T为不完整类型时，sizeof(T)给出的是0，根据代码规则，-1是不能作为数组的size的，因此，这里相当于强制编译器给出error而不是 warning)
         *
         */
        typedef char T_must_be_complete_type[sizeof(T)==0?-1:1]; //类T一定要完整，否则delete时会出错，这里当T不完整时将数组的长度置为-1，会报错
        T_must_be_complete_type dummy; //因此，这里相当于强制编译器给出error而不是 warning
        (void) dummy;
        delete value_;
        value_ = NULL;
    }

private:
    static pthread_once_t ponce_;
    static T* value_;

};



template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::value_ = NULL;


#endif //ENGINE_SINGLETON_H
