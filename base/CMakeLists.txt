set(base_SRCS
        Exception.cpp
        Mutex.cpp
        Thread.cpp
        Timestamp.cpp
        Condition.cpp
        CountDownLatch.cpp
        ThreadPool.cpp
        Date.cpp
        LogStream.cpp
        )
add_library(engine ${base_SRCS})
target_link_libraries(engine pthread)



add_subdirectory(tests)