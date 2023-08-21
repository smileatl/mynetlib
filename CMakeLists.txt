
# 最终要编译成动态库
# 脱离boost库，用C++11语言本身完成
cmake_minimum_required(VERSION 2.5)
# 项目名字
project(mynetlib)

# 添加搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/mysql)
include_directories(${PROJECT_SOURCE_DIR}/src/memory)

# 定义参与编译的源代码文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/mysql SRC_MYSQL)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/memory SRC_MEMORY)


# cmake=》makefile make，如果添加了新文件，需要重新cmake产生makefile，才能让新文件参与到编译
# 需要cmake文件右键清理所有项目并重新配置，然后清理所有项目重新生成
# mymudo最终编译成so动态库，设置动态库的路径，放在根目录的lib文件夹下面
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试信息 以及启动C++11语言标准
# -fPIC制作动态库，生成与位置无关的动态库
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -fPIC")

# 编译生成动态库mynetlib
add_library(mynetlib SHARED ${SRC_LIST} ${SRC_MYSQL} ${SRC_MEMORY})

# 目标动态库所需连接的库（这里需要连接libpthread.so）
target_link_libraries(mynetlib pthread mysqlclient)

# 设置安装的默认路径
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

# 将目录安装到 include/tiny_network 下（只安装头文件）
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include FILES_MATCHING PATTERN "*.h")

# 最终生成libmuduo.so

add_subdirectory(src/mysql/test)
add_subdirectory(src/memory/test)



