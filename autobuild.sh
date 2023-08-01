#!/bin/bash

set -e

# 如果没有build目录，创建该目录
if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

rm -rf `pwd`/build/* 

cd `pwd`/build &&
    cmake .. &&
    make

# 回到项目根目录
cd ..

# 把头文件拷贝到 /usr/include/mymuduo，因为/usr/include头文件太多
# so库拷贝到 /usr/lib    这些默认是系统的PATH
if [ ! -d /usr/include/mymuduo ]; then   
    mkdir /usr/include/mymuduo
fi

for header in `ls *.h`
do
    cp $header /usr/include/mymuduo
done

# if [ ! -d ./include ]; then 
#     mkdir ./include
# fi

# for header in `ls ./*.h`
# do
#     cp $header ./include 
# done

cp `pwd`/lib/libmymuduo.so /usr/lib

# 刷新动态库缓存，链接库如果找不到
ldconfig


