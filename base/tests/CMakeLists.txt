add_executable(MyException MyException.cpp)
target_link_libraries(MyException engine)

add_executable(MyMutex MyMutex.cpp)
target_link_libraries(MyMutex engine)

add_executable(MyTimestamp MyTimestamp.cpp)
target_link_libraries(MyTimestamp engine)

add_executable(MyThread MyThread.cpp)
target_link_libraries(MyThread engine)

add_executable(MyThreadPool MyThreadPool.cpp)
target_link_libraries(MyThreadPool engine)

add_executable(MyDate MyDate.cpp)
target_link_libraries(MyDate engine)

add_executable(MySingleton MySingleton.cpp)
target_link_libraries(MySingleton engine)

add_executable(MyAtomic MyAtomic.cpp)
target_link_libraries(MyAtomic engine)

add_executable(MyBlockingQueue MyBlockingQueue.cpp)
target_link_libraries(MyBlockingQueue engine)

add_executable(MyThreadLocalSingleton MyThreadLocalSingleton.cpp)
target_link_libraries(MyThreadLocalSingleton engine)

add_executable(MyThreadLocal MyThreadLocal.cpp)
target_link_libraries(MyThreadLocal engine)

add_executable(MyLogStream MyLogStream.cpp)
target_link_libraries(MyLogStream engine)

add_executable(MyLogging MyLogging.cpp)
target_link_libraries(MyLogging engine)

add_executable(MyTimeZone MyTimestamp.cpp)
target_link_libraries(MyTimestamp engine)

