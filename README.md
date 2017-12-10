# Engine
- rewrite from muduo base class



## 进程和线程

- 每个进程有自己独立的地址空间 (address space)
- 线程的特点是共享地址空间，从而可以高效地共享数据

## 服务器模型

### 1. single process programing model

> Reactor(基于IO multiple)
- non-blocking IO + IO multiplexing 程序的基本结构是一个事件循环 (event loop)
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
- Reactor模型的库
    - C - lighttpd
    - C - libevent
    - C++ - ACE
    - Java - NIO, Netty
    - Perl - POE
    - python - Twisted
   
> Proactor(基于异步IO)
- Proactor模型的库
    - boost.asio
    - Windows IOCP
    
    
### 2. multithread programing model
> one (event) loop per thread (multiple Reactors) + thread pool
- event loop per thread: 程序里的每个IO线程有一个event loop（或者叫 Reactor），用于处理读写和定时事件（无论周期性的还是单次的）. event loop用作non-blocking IO和定时器
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

> muduo 

- 就是采用这种模型 multiple Reactors + thread Pool
- 也可以实现
    - Reactor(single IO thread, no worker thread)
    - Reactor((single IO thread) + thread pool( restricted threads)
    - multiple Reactors( multiple IO threads, no worker thread )
    
## about thread
> Linux能同时启动多少个线程 ?
- 对于32-bit Linux，一个进程的地址空间是 4G，其中用户态能访问 3G 左右，而一个线程的默认栈 (stack) 大小是 10M，心算可知，一个进程大约最多能同时启动 300 个线程左右

> 多线程能提高并发度吗？
- 如果指的是“并发连接数”，不能。
- 假如单纯采用 thread per connection 的模型，那么并发连接数大约300，这远远低于基于事件的单线程程序所能轻松达到的并发连接数（几千上万，甚至几万）。所谓“基于事件”，指的是用 IO multiplexing event loop 的编程模型，又称 Reactor 模式。

> 多线程能提高吞吐量吗？
- 对于计算密集型服务，不能。
- 如果要在一个8核的机器上压缩100个1G的文本文件，每个core的处理能力为200MB/s，那么“每次起8个进程，一个进程压缩一个文件”与“只启动一个进程（8个线程并发压缩一个文件）”，这两种方式总耗时相当，但是第二种方式能较快的拿到第一个压缩完的文件

> 多线程能提高响应时间吗？
- 可以

>多线程如何让I/O和计算重叠
- 多线程程序如何让I/O和计算重叠，降低latency（迟延）
    - 例：日志（logging），多个线程写日志，由于文件操作比较慢，服务线程会等在IO上，让CPU空闲，增加响应时间。
    - 解决办法：单独用一个logging线程负责写磁盘文件，通过BlockingQueue提供对外接口，别的线程要写日志的时候往队列一塞就行，这样服务线程的计算和logging线程的磁盘IO就可以重叠。
    - 如果异步I/O成熟的话，可以用protator模式。

> 线程池大小的选择
- 如果池中执行任务时，密集计算所占时间比重为P（0<P<=1)，而系统一共有C个CPU，为了让C个CPU跑满而不过载，线程池大小的经验公式T=C/P，即T*P=C（让CPU刚好跑满 ）
    - 假设C=8，P=1.0，线程池的任务完全密集计算，只要8个活动线程就能让CPU饱和
    - 假设C=8，P=0.5，线程池的任务有一半是计算，一半是IO，那么T=16，也就是16个“50%繁忙的线程”能让8个CPU忙个不停。

> 线程分类
- I/O线程（这里特指网络I/O） reactor
- 计算线程  这个比较耗费CPU
- 第三方库所用线程，如logging,又比如database

