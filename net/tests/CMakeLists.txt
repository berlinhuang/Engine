add_executable(MyInetAddress  MyInetAddress.cpp)
target_link_libraries(MyInetAddress engine_net engine_base boost_unit_test_framework)
add_test(NAME MyInetAddress COMMAND MyInetAddress)

add_executable(TestEventLoop TestEventLoop.cpp)
target_link_libraries(TestEventLoop engine_net engine_base )

