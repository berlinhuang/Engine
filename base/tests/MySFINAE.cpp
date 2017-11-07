//
// Created by root on 17-11-7.
//
// Substitution Failure Is Not An Error
#include <stdio.h>
template <class T>
struct is_pointer
{
    //4个重载的 is_ptr函数
    template <class U>              static char is_ptr(U *);
    template <class U>              static char is_ptr(U (*)());
    template <class X, class Y>     static char is_ptr(Y X::*);
    static double is_ptr(...);//包括了其他的所有参数

    static T t;
    //sizeof(is_ptr(t))
    enum { value = sizeof(is_ptr(t)) == sizeof(char) };
};

struct Foo {
    int bar;
};

int main() {
    typedef int * IntPtr;
    typedef int Foo::* FooMemberPtr;
    typedef int (*FuncPtr)();

    printf("%d\n",is_pointer<IntPtr>::value);        // prints 1
    printf("%d\n",is_pointer<FooMemberPtr>::value);  // prints 1
    printf("%d\n",is_pointer<FuncPtr>::value);       // prints 1
    return 0;
}
