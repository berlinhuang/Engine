//
// Created by root on 17-10-11.
//

#include "FileUtil.h"
#include "Logging.h"

#include <fcntl.h>//::open()  O_RDONLY
//#include <unistd.h> //::close()
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h> //::fstat()
#include <boost/static_assert.hpp>


void FileUtil::AppendFile::append(const char* logline, const size_t len)
{
    size_t n = write(logline, len);
    size_t remain = len - n;
    while(remain> 0)//剩余写入字节数大于0
    {
        size_t x = write(logline+n, remain);
        if(x==0)
        {
            int err = ferror(fp_);
            if(err)
            {
                fprintf(stderr, "AppendFile::append() fiailed %s\n", strerror_tl(err));
            }
            break;
        }
        n+=x;
        remain = len-n;
    }
    writtenBytes_+= len;
}

size_t FileUtil::AppendFile::write(const char *logline, size_t len)
{
    return ::fwrite_unlocked(logline, 1, len, fp_); //不加锁的方式写入，效率高，not thread safe
}

void FileUtil::AppendFile::flush()
{
    ::fflush(fp_);
}


FileUtil::ReadSmallFile::ReadSmallFile(StringArg filename)
:fd_(::open(filename.c_str(),O_RDONLY|O_CLOEXEC)),
 err_(0)
{
    buf_[0] = '\0';
    if(fd_<0)
    {
        err_ = errno;
    }
}

FileUtil::ReadSmallFile::~ReadSmallFile()
{
    if(fd_>= 0)
    {
        ::close(fd_);
    }
}

template <typename String>
int FileUtil::ReadSmallFile::readToString(int maxSize,
                                          String *content,
                                          int64_t *fileSize,
                                          int64_t *modifyTime,
                                          int64_t *createTime)
{
    BOOST_STATIC_ASSERT(sizeof(off_t) == 8);
    assert(content != NULL);
    int err = err_;
    if(fd_>=0)
    {
        content->clear();
        if(fileSize)//如果不为空，获取文件大小
        {
            struct stat statbuf;//fstat函数用来 获取文件（普通文件，目录，管道，socket，字符，块（）的属性
            if(::fstat(fd_, &statbuf)==0)//fstat用来获取文件大小，保存到缓冲区当中
            {
                if(S_ISREG(statbuf.st_mode))
                {
                    *fileSize = statbuf.st_size;
                    content->reserve(static_cast<int>(std::min(implicit_cast<int64_t >(maxSize),*fileSize)));
                }
                else if(S_ISDIR(statbuf.st_mode))
                {
                    err = EISDIR;
                }
                if(modifyTime)
                {
                    *modifyTime = statbuf.st_mtime;
                }
                if(createTime)
                {
                    *createTime = statbuf.st_ctime;
                }
            }
            else
            {
                err = errno;
            }
        }

        while(content->size()<implicit_cast<size_t>(maxSize))
        {
            size_t  toRead = std::min(implicit_cast<size_t>(maxSize)-content->size(),sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);
            if(n>0)
            {
                content->append(buf_,n);
            }
            else
            {
                if(n>-0)
                {
                    err= errno;
                }
                break;
            }
        }
    }
    return err;
}


int FileUtil::ReadSmallFile::readToBuffer(int *size)
{
    int err = err_;
    if(fd_>= 0)
    {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1,0);
        if(n>=0)
        {
            if(size)
            {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\n';
        }
        else
        {
            err = errno;
        }
    }
    return err;
}




template int FileUtil::readFile(
        StringArg filename,
        int maxSize,
        string* content,
        int64_t*,
        int64_t*,
        int64_t*);

template int FileUtil::ReadSmallFile::readToString(
        int maxSize,
        string *content,
        int64_t*,
        int64_t*,
        int64_t*);




















