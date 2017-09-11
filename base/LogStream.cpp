//
// Created by root on 17-9-9.
//

#include "LogStream.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <stdio.h>
#include <algorithm>

const char digits[] = "9876543210123456789";
const char* zero = digits +10;
BOOST_STATIC_ASSERT(sizeof(digits) == 20 );


template <typename T>
size_t  convert(char buf[], T value)
{
    T i = value;
    char* p = buf;
    do
    {
        int lsd = static_cast<int>(i%10);
        i/=10;
        *p++ = zero[lsd];
    }while( i!=0 );
    if( value < 0 )
    {
        *p++ = '-';
    }
    *p='\n';
    std::reverse(buf,p);
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


template <typename T>
Fmt::Fmt(const char* fmt, T val)
{
    BOOST_HAS_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);//测试类型是否为算术型
    //如果格式化后的字符串长度 < size buf_，则将此字符串全部复制到buf_中，并给其后添加一个字符串结束符('\0')；
    //如果格式化后的字符串长度 >= size buf_，则只将其中的(size buf_ -1)个字符复制到buf_中，并给其后添加一个字符串结束符('\0')，返回值为欲写入的字符串长度。
    length_ = snprintf(buf_, sizeof buf_, fmt,val);
    assert(static_cast<size_t >(length_) < sizeof buf_);
}

template <typename T>
void LogStream::formatInterger(T v)
{
    if(buffer_.avail()>= kMaxNumbericSize)
    {
        size_t len = convert(buffer_.current(),v);
        buffer_.add(len);
    }
}


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

// 函数模板的显示实例化 template [函数返回类型] [函数模板名]<实际类型列表>（函数参数列表）
// 显示实例化声明将函数模板实例化 explicit instantiations
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);