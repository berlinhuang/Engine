cmake_minimum_required(VERSION 3.8)
project(Engine)

set(CMAKE_CXX_STANDARD 11)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("Bin: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(ENV{USE_EPOLL} 1)

message("USE_EPOLL: " $ENV{USE_EPOLL})
#
#set(SOURCE_FILES base/Thread.cpp base/Thread.h)
#add_library(Thread ${SOURCE_FILES})
#
#add_library(Exception base/Exception.cpp base/Exception.h)
#add_library(Timestamp base/Timestamp.cpp base/Timestamp.h)
#
#set(MYTHREAD base/tests/MyTreahd.cpp)
#add_executable(MyTreahd ${MYTHREAD})
#target_link_libraries(MyTreahd Thread pthread Exception)
#
#
#add_executable(MyException base/tests/MyException.cpp)
#target_link_libraries(MyException Exception)
#
#add_executable(MyTimestamp base/tests/MyTimestamp.cpp)
#target_link_libraries(MyTimestamp Timestamp)
add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(examples)

add_subdirectory(recipes)
