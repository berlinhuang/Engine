//
// Created by root on 17-9-8.
//

//每个线程一个Singleton

#ifndef ENGINE_THREADLOCALSINGLETON_H
#define ENGINE_THREADLOCALSINGLETON_H

#include <assert.h>

class ThreadLocalSingleton
{
public:
    static T& instance()
    {
        if(!t_value_)
        {
            t_value_ = new T();
            deleter_.set(t_value_);
        }
        return *t_value_;
    }

    static T* pointer()
    {
        return t_value_;
    }

private:
    ThreadLocalSingleton();
    ~ThreadLocalSingleton();

    static void destructor(void* obj)
    {
        assert(obj=t_value_);
        typedef char T_must_be_complete_type[sizeof(T)==0?-1:1];
        T_must_be_complete_type dummy;
        (void) dummy;
        delete t_value_;
        t_value_ = 0;
    }

    class Deleter
    {
    public:
        Deleter()
        {
            pthread_key_create(&pkey_,&ThreadLocalSingleton::destructor);
        }
        ~Deleter()
        {
            pthread_key_delete(pkey_);
        }

        void set(T* newobj)
        {
            assert(pthread_getspecific(pkey_)==NULL);
            pthread_setspecific(pkey_,newObj);
        }
        pthread_key_t pkey_;
    };



    static __thread T* t_value_;
    static Deleter deleter_;

};


#endif //ENGINE_THREADLOCALSINGLETON_H

template <typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template<typename T>
typename THreadLocalSingleton<T>::Deleter ThreadLocalSingleton<T>::deleter_;