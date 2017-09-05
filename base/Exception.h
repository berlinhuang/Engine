//
// Created by root on 17-9-2.
//

#ifndef MYTHREAD_EXCEPTION_H
#define MYTHREAD_EXCEPTION_H
#include <exception>
#include <string>
using std::string;
class Exception:public std::exception
{
public:
    explicit Exception(const char* what);
    explicit Exception( const string& what);
    virtual const char* what() const throw();
    const char* stackTrace() const throw();
    virtual ~Exception() throw();

private:
    void fillStackTrace();

    string message_;
    string stack_;

};




#endif //MYTHREAD_EXCEPTION_H
