include_directories(/usr/local/protobuf/include)

add_library(protobuf_codec codec.cpp)

add_executable(protobuf_server server.cpp)