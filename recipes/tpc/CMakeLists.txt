
add_executable(echo  TcpStream.cpp Socket.cpp InetAddress.cpp Acceptor.cpp echo.cpp)
target_link_libraries(echo pthread)