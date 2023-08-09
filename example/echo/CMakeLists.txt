set(SRC_LIST testserver.cc)

add_executable(echotest ${SRC_LIST})
target_link_libraries(echotest mynetlib pthread)

add_definitions(-std=c++17 -g)