# 定义了一系列源文件 http_SRCS，它包含了实现 HTTP 功能所需的多个 .cc 文件
set(http_SRCS
  HttpServer.cc
  HttpResponse.cc
  HttpContext.cc
  )

# 使用 add_library() 函数创建了一个名为 muduo_http 的库，并将之前定义的变量 http_SRCS 作为源文件列表传递给该函数。这个函数会生成一个静态库，因为没有指定库类型，默认是静态库。
add_library(mymuduo_http ${http_SRCS})
# 使用 target_link_libraries() 函数将 muduo_net 库链接到 muduo_http。
target_link_libraries(mymuduo_http mymuduo_net)

# 使用 install() 函数将目标 muduo_http 安装到目录 lib 中
install(TARGETS mymuduo_http DESTINATION lib)
# 使用 set() 函数定义了一个变量 HEADERS，其中包含了四个头文件 HttpContext.h、HttpRequest.h、HttpResponse.h 和 HttpServer.h
set(HEADERS
  HttpContext.h
  HttpRequest.h
  HttpResponse.h
  HttpServer.h
  )
# 使用 install() 函数将这些头文件安装到目录 include/muduo/net/http
install(FILES ${HEADERS} DESTINATION include/mymuduo/net/http)

# 使用条件判断语句 if(MUDUO_BUILD_EXAMPLES) 来检查是否需要构建示例程序
if(MUDUO_BUILD_EXAMPLES)
# 使用 add_executable() 函数创建一个可执行文件 httpserver_test，并将源文件 tests/HttpServer_test.cc 传递给该函数
add_executable(myhttpserver_test tests/HttpServer_test.cc)
# 使用 target_link_libraries() 函数将 muduo_http 库链接到 httpserver_test。
target_link_libraries(myhttpserver_test mymuduo_http)

# 如果 BOOSTTEST_LIBRARY 存在，
if(BOOSTTEST_LIBRARY)
# 类似地使用 add_executable() 函数创建另一个可执行文件 httprequest_unittest
add_executable(myhttprequest_unittest tests/HttpRequest_unittest.cc)
# 并将源文件 tests/HttpRequest_unittest.cc 以及 boost_unit_test_framework 链接到该文件。
target_link_libraries(myhttprequest_unittest mymuduo_http boost_unit_test_framework)
endif()

endif()


# add_subdirectory(tests)

add_definitions(-std=c++17)

# 这个 CMake 文件用于构建一个名为 muduo_http 的库，并安装到指定目录。同时，如果需要构建示例程序，则会生成相应的可执行文件，并链接相关的库。请根据你的具体需求和项目结构进行适当修改和扩展。

