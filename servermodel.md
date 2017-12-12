### EventLoopThreadPool


### EventLoopThread

- 任何一个线程，只要创建并运行了EventLoop，就是一个IO线程。 EventLoopThread类就是一个封装好了的IO线程。
- EventLoopThread的工作流程为： 
    1. 在主线程创建EventLoopThread对象。 
    2. 主线程调用EventLoopThread.start()，启动EventLoopThread中的线程（称为IO线程），并且主线程要等待IO线程创建完成EventLoop对象。 
    3. IO线程调用threadFunc创建EventLoop对象。通知主线程已经创建完成。 
    4. 主线程返回创建的EventLoop对象。