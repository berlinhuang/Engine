set(http_SRCS
  HttpServer.cc
  HttpResponse.cc
  HttpContext.cc
  )

add_library(engine_http ${http_SRCS})
target_link_libraries(engine_http engine_base engine_net)

install(TARGETS engine_http DESTINATION lib)
set(HEADERS
  HttpRequest.h
  HttpResponse.h
  HttpServer.h
  )
#install(FILES ${HEADERS} DESTINATION include/engine/net/http)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
add_executable(httpserver_test tests/HttpServer_test.cc)
target_link_libraries(httpserver_test engine_http)


if(BOOSTTEST_LIBRARY)
    add_executable(httprequest_unittest tests/HttpRequest_unittest.cc)
    target_link_libraries(httprequest_unittest engine_net engine_http boost_unit_test_framework)
endif()

endif()

# add_subdirectory(tests)
