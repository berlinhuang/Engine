include_directories(/usr/local/protobuf/include)

add_library(protobuf_codec codec.cpp)
target_link_libraries(protobuf_codec protobuf engine_net engine_base z)

# protoc  --cpp_out $DST_DIR -I=$SRC_DIR $SRC_DIR/query.proto
# -I指定源目的文件夹
# --cpp_out 是生成代码的目的地 一般和src dir相同
find_program(PROTOC protoc /usr/local/protobuf/bin/)

#
#add_custom_command(OUTPUT query.pb.cpp query.pb.h
#        COMMAND ${PROTOC}
#        ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/query.proto -I${CMAKE_CURRENT_SOURCE_DIR}
#        DEPENDS query.proto
#        VERBATIM )

set_source_files_properties(query.pb.cpp PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-shadow")
include_directories(${PROJECT_BINARY_DIR})

add_library(query_proto query.pb.cpp)
link_directories(/usr/local/protobuf/lib/)
target_link_libraries(query_proto protobuf pthread)

set_source_files_properties(query.pb.cpp PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-shadow")
include_directories(${PROJECT_BINARY_DIR})


add_executable(protobuf_codec_test codec_test.cpp)
set_target_properties(protobuf_codec_test PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_codec_test protobuf_codec query_proto)


add_executable(protobuf_dispatcher_lite_test dispatcher_lite_test.cpp)
set_target_properties(protobuf_dispatcher_lite_test PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_dispatcher_lite_test query_proto)


add_executable(protobuf_dispatcher_test dispatcher_test.cpp)
set_target_properties(protobuf_dispatcher_test PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_dispatcher_test query_proto)


add_executable(protobuf_server server.cpp)
set_target_properties(protobuf_server PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_server protobuf_codec query_proto)


add_executable(protobuf_client client.cpp)
set_target_properties(protobuf_client PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_client protobuf_codec query_proto)
