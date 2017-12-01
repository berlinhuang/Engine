//
// Created by root on 17-9-9.
//

#ifndef ENGINE_TYPE_H
#define ENGINE_TYPE_H

#include <string>

using std::string;

template <typename To, typename From>
inline To implicit_cast(From const &f)
{
    return f;
}

#endif //ENGINE_TYPE_H
