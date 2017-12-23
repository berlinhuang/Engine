add_executable(balancer balancer.cpp)
target_link_libraries(balancer engine_base engine_net)

add_executable(tcprelay tcprelay.cpp)
target_link_libraries(tcprelay engine_base engine_net)

add_executable(socks4a socks4a.cpp)
target_link_libraries(socks4a engine_base engine_net)

