set(net_SRCS
        InetAddress.cpp
        SocketsOps.cpp
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        poller/DefaultPoller.cpp
        poller/PollPoller.cpp
        poller/EPollPoller.cpp
        Timer.cpp
        TimerQueue.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        Socket.cpp
        Acceptor.cpp
        TcpServer.cpp
        Buffer.cpp
        TcpConnection.cpp
        )
add_library(engine_net ${net_SRCS})
target_link_libraries(engine_net pthread)

#set_target_properties(engine PROPERTIES COMPILE_FLAGS "-std=c++0x")

add_subdirectory(tests)