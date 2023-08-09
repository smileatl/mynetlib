#!/bin/bash

set -e

# 如果没有build目录，创建该目录
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

# 把头文件拷贝到 /usr/include/mynetlib，因为/usr/include头文件太多
# so库拷贝到 /usr/lib    这些默认是系统的PATH
if [ ! -d /usr/local/include/mynetlib ]; then   
    mkdir /usr/local/include/mynetlib
fi

if [ ! -d ./include ]; then 
    mkdir ./include
fi


rm -rf `pwd`/build/* 

cd `pwd`/build &&
    cmake .. &&
    make install

# 回到项目根目录
cd ..

cp `pwd`/include/* -r /usr/local/include/mynetlib

cp `pwd`/lib/libmynetlib.so /usr/local/lib

# for header in `ls ./*.h`
# do
#     cp $header ./include 
# done

# 刷新动态库缓存，链接库如果找不到
ldconfig


