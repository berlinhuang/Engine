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
        Logging.cpp
        TimeZone.cpp
        FileUtil.cpp
        LogFile.cpp
        ProcessInfo.cpp
        )
add_library(engine_base ${base_SRCS})
target_link_libraries(engine_base pthread)

#set_target_properties(engine PROPERTIES COMPILE_FLAGS "-std=c++0x")



add_subdirectory(tests)