add_executable(MyInetAddress  MyInetAddress.cpp)
target_link_libraries(MyInetAddress engine_net engine_base boost_unit_test_framework)
add_test(NAME MyInetAddress COMMAND MyInetAddress)

add_executable(TestEventLoop TestEventLoop.cpp)
target_link_libraries(TestEventLoop engine_net engine_base )

add_executable(TestEventLoop3 TestEventLoop3.cpp)
target_link_libraries(TestEventLoop3 engine_net engine_base )

add_executable(MyTimeQueue MyTimeQueue.cpp)
target_link_libraries(MyTimeQueue engine_net engine_base )

add_executable(crashLoopTest crashLoopTest.cpp)
target_link_libraries(crashLoopTest engine_net engine_base )


add_executable(MyEventLoopThread MyEventLoopThread.cpp)
target_link_libraries(MyEventLoopThread engine_net engine_base)


add_executable(MyAcceptor MyAcceptor.cpp)
target_link_libraries(MyAcceptor engine_net engine_base)

add_executable(MyNewConnection MyNewConnection.cpp)
target_link_libraries(MyNewConnection engine_net engine_base)

add_executable(MyNewConnection2 MyNewConnection2.cpp)
target_link_libraries(MyNewConnection2 engine_net engine_base)


add_executable(MyBuffer  MyBuffer.cpp)
target_link_libraries(MyBuffer engine_net engine_base boost_unit_test_framework)
add_test(NAME MyBuffer COMMAND MyBuffer)


add_executable(MyEventLoopThreadPool MyEventLoopThreadPool.cpp)
target_link_libraries( MyEventLoopThreadPool engine_net engine_base)

add_executable(MyTcpClient_reg1 MyTcpClient_reg1.cpp)
target_link_libraries( MyTcpClient_reg1 engine_net engine_base)

add_executable(MyTcpClient_reg2 MyTcpClient_reg2.cpp)
target_link_libraries( MyTcpClient_reg2 engine_net engine_base)


add_executable(MyEventLoopThread1 MyEventLoopThread1.cpp)
target_link_libraries(MyEventLoopThread1 engine_net engine_base)