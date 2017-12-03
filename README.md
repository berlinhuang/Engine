# Engine
- rewrite from muduo base class



## 进程和线程

- 每个进程有自己独立的地址空间 (address space)
- 线程的特点是共享地址空间，从而可以高效地共享数据

## 服务器模型

### 1. single process programing model

> Reactor = non-blocking IO + IO multiplexing 程序的基本结构是一个事件循环 (event loop)
``` c++
   while (!done)
   {
     int timeout_ms = max(1000, getNextTimedCallback());
     int retval = ::poll(fds, nfds, timeout_ms);
     if (retval < 0) {
       处理错误
     } else {
       处理到期的 timers
       if (retval > 0) {
         处理 IO 事件
       }
     }
   }
```
- Reactor释义"反应堆"，是一种事件驱动机制。和普通函数调用的不同之处在于：应用程序不是主动的调用某个API完毕处理。而是恰恰相反。
- Reactor逆置了事件处理流程，应用程序须要提供对应的接口并注冊到Reactor上，假设对应的时间发生，Reactor将主动调用应用程序注冊的接口，这些接口又称为"回调函数"。

> Reactor模型的库

- C - lighttpd
- C - libevent
- C++ - ACE
- Java - NIO, Netty
- Perl - POE
- python - Twisted
   
> Proactor
- boost.asio
- Windows IOCP
    
    
### 2. multithread programing model
> one (event) loop per thread + thread pool
- event loop per thread: 程序里的每个 IO 线程有一个 event loop （或者叫 Reactor），用于处理读写和定时事件（无论周期性的还是单次的）. event loop 用作 non-blocking IO 和定时器
- thread pool: 用来做计算，具体可以是任务队列或消费者-生产者队列
``` C++
// 下面实现了一个简单的固定数目的线程池

blocking_queue<boost::function<void()> > taskQueue;  // 线程安全的阻塞队列
void worker_thread()
{
  while (!quit) {
    boost::function<void()> task = taskQueue.take();  // this blocks
    task();  // 在产品代码中需要考虑异常处理
  }
}

// 启动容量为 N 的线程池：
int N = num_of_computing_threads;
for (int i = 0; i < N; ++i) {
  create_thread(&worker_thread);  // 伪代码：启动线程
}

// 使用
Foo foo;//Foo has calc() member
boost::function<void()> task = boost::bind(&Foo::calc, this);
taskQueue.post(task);

```

> muduo 就是采用这种模型
    
    



