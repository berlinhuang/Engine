add_library( tpc TcpStream.cpp Socket.cpp InetAddress.cpp Acceptor.cpp)

add_executable(echo echo.cpp)
target_link_libraries(echo tpc pthread)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client tpc pthread)

