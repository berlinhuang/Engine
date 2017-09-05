add_executable(MyException MyException.cpp)
target_link_libraries(MyException engine)

add_executable(MyMutex MyMutex.cpp)
target_link_libraries(MyMutex engine)

add_executable(MyTimestamp MyTimestamp.cpp)
target_link_libraries(MyTimestamp engine)

add_executable(MyThread MyThread.cpp)
target_link_libraries(MyThread engine)