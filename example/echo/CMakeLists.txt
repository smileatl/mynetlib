set(SRC_LIST testserver.cc)

add_executable(echotest ${SRC_LIST})
target_link_libraries(echotest mymuduo pthread)

add_definitions(-std=c++17 -g)