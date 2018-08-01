#if(BOOSTPO_LIBRARY)
    add_executable(ttcp_blocking ttcp_blocking.cpp common.cpp main.cpp)
    target_link_libraries(ttcp_blocking engine_base engine_net boost_program_options)
    set_target_properties(ttcp_blocking PROPERTIES COMPILE_FLAGS "-Wno-error=old-style-cast -Wno-error=conversion")

    add_executable(ttcp_engine ttcp.cpp common.cpp main.cpp)
    target_link_libraries(ttcp_engine engine_base engine_net boost_program_options) #linux系统下,编译选项需加上 -lboost_program_options
#endif()