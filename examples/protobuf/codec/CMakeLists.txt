include_directories(/usr/local/protobuf/include)

add_library(protobuf_codec codec.cpp)
target_link_libraries(protobuf_codec protobuf engine_net engine_base z)

# protoc  --cpp_out $DST_DIR -I=$SRC_DIR $SRC_DIR/query.proto


#set(PROTOCCMD /usr/local/protobuf/bin/protoc)
#add_custom_command(OUTPUT query.pb.cpp query.pb.h
#        COMMAND ${PROTOCCMD}
#        ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/query.proto -I${CMAKE_CURRENT_SOURCE_DIR}
#        DEPENDS query.proto
#        VERBATIM )

set_source_files_properties(query.pb.cpp PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-shadow")
include_directories(${PROJECT_BINARY_DIR})

add_library(query_proto query.pb.cpp)

link_directories(/usr/local/protobuf/lib/)
target_link_libraries(query_proto protobuf pthread)


add_executable(protobuf_server server.cpp)
set_target_properties(protobuf_server PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_server protobuf_codec query_proto)