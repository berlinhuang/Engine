//
// Created by root on 17-10-12.
//

#ifndef ENGINE_STRINGPIECE_H
#define ENGINE_STRINGPIECE_H

#include <string.h>
#include <string>
using std::string;


class StringArg
{
public:
    StringArg(const char* str):str_(str){}
    StringArg(const string& str):str_(str.c_str()){}
    const char* c_str() const { return str_; }
private:
    const char* str_;
};



class StringPiece {
private:
    const char *ptr_;
    int length_;

public:


    StringPiece() : ptr_(NULL), length_(0) {}

    StringPiece(const char *str)
            : ptr_(str),
              length_(static_cast<int>(strlen(ptr_))) {}

    StringPiece(const unsigned char *str)
            : ptr_(reinterpret_cast<const char *>(str)),
              length_(static_cast<int>(strlen(ptr_))) {}

    StringPiece(const string &str)
            : ptr_(str.data()),
              length_(static_cast<int>(str.size())) {}

    StringPiece(const char *offset, int len)
            : ptr_(offset),
              length_(len) {}

    const char *data() const {
        return ptr_;
    }

    int size() const {
        return length_;
    }

    bool empty() const {
        return length_ == 0;
    }

    const char *begin() const {
        return ptr_;
    }

    const char *end() const {
        return ptr_ + length_;
    }

    void clear()
    {
        ptr_ = NULL;
        length_ = 0;
    }

    void set(const char* buffer, int len)
    {
        ptr_ = buffer;
        length_ = len;
    }

    void set(const char* str)
    {
        ptr_ = str;
        length_ = static_cast<int>(strlen(str));
    }

    void set( const void* buffer, int len)
    {
        ptr_ = reinterpret_cast<const char*>(buffer);
        length_ = len;
    }

    char operator[](int i) const
    {
        return ptr_[i];
    }


    string as_string() const {
        return string(data(), size());
    }









};





#endif //ENGINE_STRINGPIECE_H