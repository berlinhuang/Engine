//
// Created by root on 17-9-2.
//
#include "../Exception.h"
#include <stdio.h>
class Bar
{
public:
    void test()
    {
        throw Exception("oops");
    }
};


void foo()
{
    Bar b;
    b.test();
}
int main()
{
    try{
    foo();
    }
    catch (const Exception& ex){
        printf("reason:%s\n",ex.what());
        printf("stack trace:%s\n",ex.stackTrace());

    }
}