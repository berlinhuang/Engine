set(net_SRCS
        InetAddress.cpp
        SocketsOps.cpp
        EventLoop.cpp
        )
add_library(engine_net ${net_SRCS})
target_link_libraries(engine_net pthread)

#set_target_properties(engine PROPERTIES COMPILE_FLAGS "-std=c++0x")



add_subdirectory(tests)