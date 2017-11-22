add_executable(MyInetAddress  MyInetAddress.cpp)
target_link_libraries(MyInetAddress engine_net engine_base boost_unit_test_framework)
add_test(NAME MyInetAddress COMMAND MyInetAddress)

add_executable(TestEventLoop TestEventLoop.cpp)
target_link_libraries(TestEventLoop engine_net engine_base )

add_executable(TestEventLoop3 TestEventLoop3.cpp)
target_link_libraries(TestEventLoop3 engine_net engine_base )

add_executable(MyTimeQueue MyTimeQueue.cpp)
target_link_libraries(MyTimeQueue engine_net engine_base )