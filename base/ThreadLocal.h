//
// Created by root on 17-9-8.
//
//线程局部数据


//提供线程私有的全局变量  线程特定数据(Thread-specific Data，或TSD)
//POSIX线程库通过四个函数操作线程特定数据，分别是
// pthread_key_create，
// pthread_key_delete，
// pthread_getspecific，//同一个线程中不同函数间共享数据
// pthread_setspecific
#ifndef ENGINE_THREADLOCAL_H
#define ENGINE_THREADLOCAL_H

#include <pthread.h>

template <typename T>
class ThreadLocal
{
public:
    ThreadLocal()
    {
        pthread_key_create(&pkey_, &ThreadLocal::destructor);
    }

    ~ThreadLocal()
    {
        pthread_key_delete(pkey_);
    }
    //获取线程特定数据
    T& value()
    {
        T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));//返回的是与 pkey_ 相关联数据的指针
        if(!perThreadValue)
        {
            T *newObj = new T();
            pthread_setspecific(pkey_,newObj);//将 newObj 的值 (不是锁指的内容) 与pkey 相关联
            perThreadValue = newObj;
        }
        return *perThreadValue;
    }

private:
    static void destructor(void *x)
    {
        T *obj = static_cast<T*>(x);
        typedef  char T_must_be_complete_type[sizeof(T) == 0? -1:1];
        T_must_be_complete_type dummy;
        (void) dummy;
        delete  obj;
    }

private:
    pthread_key_t pkey_;


};



#endif //ENGINE_THREADLOCAL_H
