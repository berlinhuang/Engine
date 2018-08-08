add_library( tpc TcpStream.cpp Socket.cpp InetAddress.cpp Acceptor.cpp)

add_executable(echo echo.cpp)
target_link_libraries(echo tpc pthread)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client tpc pthread)

add_executable(tpc_roundtrip_udp roundtrip_udp.cpp)
target_link_libraries(tpc_roundtrip_udp tpc pthread)
