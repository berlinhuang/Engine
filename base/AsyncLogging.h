//
// Created by root on 17-12-17.
//

#ifndef ENGINE_ASYNCLOGGING_H
#define ENGINE_ASYNCLOGGING_H


#include "BlockingQueue.h"
//#include "BoundedBlockingQueue.h"
#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

class AsyncLogging {

public:

    AsyncLogging(const string& basename,
                 size_t rollSize,
                 int flushInterval = 3);

    ~AsyncLogging()
    {
        if (running_)
        {
            stop();
        }
    }
    // 供前端生产者线程调用（日志数据写到缓冲区）
    void append(const char* logline, int len);

    void start()
    {
        running_ = true;
        thread_.start();// 日志线程启动
        latch_.wait();
    }

    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:

    // declare but not define, prevent compiler-synthesized functions
    AsyncLogging(const AsyncLogging&);  // ptr_container
    void operator=(const AsyncLogging&);  // ptr_container

    void threadFunc();                // 供后端消费者线程调用（将数据写到日志文件）

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef boost::ptr_vector<Buffer> BufferVector;
    typedef BufferVector::auto_type BufferPtr;

    const int flushInterval_;       // 超时时间，在flushinterval_秒内，缓冲区没写满，仍将缓冲区中的数据写到文件中
    bool running_;
    string basename_;
    size_t rollSize_;
    Thread thread_;
    CountDownLatch latch_;          //用于等待线程启动
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;       // 当前缓冲区
    BufferPtr nextBuffer_;          // 预备缓冲区
    BufferVector buffers_;          // 待写入文件的已满的缓冲区
};


#endif //ENGINE_ASYNCLOGGING_H
