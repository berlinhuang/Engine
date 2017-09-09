//
// Created by root on 17-9-9.
//

#include "LogStream.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <stdio.h>

template <typename T>
Fmt::Fmt(const char* fmt, T val)
{
    BOOST_HAS_STATIC_ASSERT(boost::is_arithmetic<T>::value == true);//测试类型是否为算术型
    //如果格式化后的字符串长度 < size buf_，则将此字符串全部复制到buf_中，并给其后添加一个字符串结束符('\0')；
    //如果格式化后的字符串长度 >= size buf_，则只将其中的(size buf_ -1)个字符复制到buf_中，并给其后添加一个字符串结束符('\0')，返回值为欲写入的字符串长度。
    length_ = snprintf(buf_, sizeof buf_, fmt,val);
    assert(static_cast<size_t >(length_) < sizeof buf_);
}