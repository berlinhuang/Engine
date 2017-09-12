//
// Created by root on 17-9-9.
//

#include "LogStream.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <stdio.h>
#include <algorithm>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;//上面的数组偏移第9位，值为0
BOOST_STATIC_ASSERT(sizeof(digits) == 20 );


const char digitsHex[] = "0123456789ABCDEF";
BOOST_STATIC_ASSERT(sizeof digitsHex == 17);


template <typename T>
size_t  convert(char buf[], T value)//这个函数实现将一个整数转换成字符串
{
    T i = value;
    char* p = buf;
    do
    {//lsd意思是last digit，最后一位数
        int lsd = static_cast<int>(i%10); //最后一位，如输入123，第一次就取3
        i/=10;
        *p++ = zero[lsd];//同时由于p指向buf，p++，那么会得到一个逆转的字符串，321
    }while( i!=0 );
    if( value < 0 )//如果小于0，加个负号
    {
        *p++ = '-';
    }
    *p='\0'; //加上\0
    std::reverse(buf,p); //由于上面得到的是逆序，所以reverse一下
    return p-buf;
}


size_t  convertHex(char buf[], uintptr_t value)
{
    uintptr_t  i = value;
    char* p = buf;

    do
    {
        int lsd = static_cast<int>(i%16);
        i/=16;
        *p++=digitsHex[lsd];
    }while(i!=0);
    *p='\0'; //加上\0
    std::reverse(buf,p); //由于上面得到的是逆序，所以reverse一下
    return p-buf;
}



//类模板的显示实例化 template class [类模板名]<实际类型列表>
//对于类模板而言，不管是否生成一个模板类的对象，都可以直接通过显示实例化声明将类模板实例化
template class FixedBuffer<kSmallBuffer>;//explicit instantiation. 显式实例化
template class FixedBuffer<kLargeBuffer>;

//template <int SIZE>
//void char* FixedBuffer<SIZE>

template <int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void LogStream::formatInterger(T v)
{
    if(buffer_.avail()>= kMaxNumbericSize)
    {
        size_t len = convert(buffer_.current(),v);//通过调用convert把整数转化成字符串
        buffer_.add(len);
    }
}



//下面这些<<最终都要调用formatIneteger()将字符串转换成整数。
LogStream& LogStream::operator<<(int v)
{
    formatInterger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInterger(v);
    return *this;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////

//把整数按照T类型格式化到buffer中
template <typename T>
Fmt::Fmt(const char* fmt, T val)
{
    BOOST_HAS_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);//测试类型是否为算术型
    //如果格式化后的字符串长度 < size buf_，则将此字符串全部复制到buf_中，并给其后添加一个字符串结束符('\0')；
    //如果格式化后的字符串长度 >= size buf_，则只将其中的(size buf_ -1)个字符复制到buf_中，并给其后添加一个字符串结束符('\0')，返回值为欲写入的字符串长度。
    length_ = snprintf(buf_, sizeof buf_, fmt,val);
    assert(static_cast<size_t >(length_) < sizeof buf_);
}


// 函数模板的显示实例化 template [函数返回类型] [函数模板名]<实际类型列表>（函数参数列表）
// 显示实例化声明将函数模板实例化 explicit instantiations
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);