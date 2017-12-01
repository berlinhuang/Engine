//
// Created by root on 17-11-24.
//

#ifndef ENGINE_BUFFER_H
#define ENGINE_BUFFER_H

#include "./../base/StringPiece.h"
#include <vector>
#include <assert.h>
#include "./../base/Type.h"


/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
    :buffer_(kCheapPrepend+initialSize),
     //
     readerIndex_(kCheapPrepend),
     writerIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }
////////////////////////////////
    size_t readableBytes() const
    {
        return writerIndex_-readerIndex_;
    }
    size_t writableBytes() const
    {
        return buffer_.size()-writerIndex_;
    }
    size_t prependableBytes() const
    {
        return readerIndex_;
    }
////////////////////////////////

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }
////////////////////////////////

    char* begin()
    {
        return &*buffer_.begin();
    }
    const char* beginWrite() const
    {
        return begin() + writerIndex_;
    }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }
////////////////////////////////
    void append(const char* /*restrict*/ data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());//
        hasWritten(len);
    }

    void append(const StringPiece& str)
    {
        append(str.data(), str.size());
    }

//////////////////////////////////
    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    const char* peek() const
    { return begin() + readerIndex_; }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }
    //Buffer中所有数据以字符串形式取走
    string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        string result(peek(), len);
        retrieve(len);
        return result;
    }

    string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }


    /// Read data directly into buffer.
    ///
    /// It may implement with readv(2)
    /// @return result of read(2), @c errno is saved
    ssize_t readFd(int fd, int* savedErrno);


private:
    const char* begin() const
    {
        return &*buffer_.begin();
    }

    char* beginWrite()
    {
        return begin() + writerIndex_;
    }


    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            // FIXME: move readable data
            buffer_.resize(writerIndex_+len);
        }
        else
        {
            // move readable data to the front, make space inside buffer
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin()+readerIndex_, begin()+writerIndex_, begin()+kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }



private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;


    static const char kCRLF[];

};


#endif //ENGINE_BUFFER_H
