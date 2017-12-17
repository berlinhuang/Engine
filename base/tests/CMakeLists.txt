add_executable(MyException MyException.cpp)
target_link_libraries(MyException engine_base)

add_executable(MyMutex MyMutex.cpp)
target_link_libraries(MyMutex engine_base)

add_executable(MyTimestamp MyTimestamp.cpp)
target_link_libraries(MyTimestamp engine_base)

add_executable(MyThread MyThread.cpp)
target_link_libraries(MyThread engine_base)

add_executable(MyThreadPool MyThreadPool.cpp)
target_link_libraries(MyThreadPool engine_base)

add_executable(MyDate MyDate.cpp)
target_link_libraries(MyDate engine_base)

add_executable(MySingleton MySingleton.cpp)
target_link_libraries(MySingleton engine_base)

add_executable(MyAtomic MyAtomic.cpp)
target_link_libraries(MyAtomic engine_base)

add_executable(MyBlockingQueue MyBlockingQueue.cpp)
target_link_libraries(MyBlockingQueue engine_base)

add_executable(MyThreadLocalSingleton MyThreadLocalSingleton.cpp)
target_link_libraries(MyThreadLocalSingleton engine_base)

add_executable(MyThreadLocal MyThreadLocal.cpp)
target_link_libraries(MyThreadLocal engine_base)

add_executable(MyLogStream MyLogStream.cpp)
target_link_libraries(MyLogStream engine_base)

add_executable(MyLogging MyLogging.cpp)
target_link_libraries(MyLogging engine_base)

add_executable(MyTimeZone MyTimeZone.cpp)
target_link_libraries(MyTimeZone engine_base)

add_executable(MyFileUtil MyFileUtil.cpp)
target_link_libraries(MyFileUtil engine_base)

add_executable(MyLogFile MyLogFile.cpp)
target_link_libraries(MyLogFile engine_base)

add_executable(MyProcessInfo MyProcessInfo.cpp)
target_link_libraries(MyProcessInfo engine_base)

add_executable(MySFINAE  MySFINAE.cpp)

add_executable(MyAsyncLogging MyAsyncLogging.cpp)
target_link_libraries(MyAsyncLogging engine_base)