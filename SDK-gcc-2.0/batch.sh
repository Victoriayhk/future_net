#!/bin/bash

SCRIPT=$(readlink -f "$0") #得到此执行文件的文件名（带绝对路径）
BASEDIR=$(dirname "$SCRIPT") #得到此执行文件的绝对路径
cd $BASEDIR #进入这个路径

#[ -d FILE ] 如果 FILE 存在且是一个目录则为真
#[ -f FILE ] 如果 FILE 存在且是一个普通文件则为真
if [ ! -d future_net ] || [ ! -f readme.txt ]
then
    echo "ERROR: $BASEDIR is not a valid directory of SDK-gcc for future_net."
    echo "  Please run this script in a regular directory of SDK-gcc."
    exit -1
fi

tmp=$(cmake --version 2>&1) #把标准错误输出2重定向到标准输出1
if [ $? -ne 0 ] #$?表示上一次程序退出值，0表示没有错误，其他任何值表明有错误；-ne 表示 不等于 
then
    echo "ERROR: You should install cmake(2.8 or later) first."
    echo "  Please run 'sudo at-get install cmake' to install it."
    echo "  or goto https://cmake.org to download and install it."
    exit
fi

#编译
# rm -fr bin
# mkdir bin
# rm -fr build
# mkdir build
cd build
cmake ../future_net
make

#打包
cd ..
mkdir code
cp -r future_net code/future_net
cd code/future_net
rm -fr lib
rm -f CMakeLists.txt
rm -f future_net.cpp
rm -f io.cpp
cd ..
tar -zcPf future_net.tar.gz *
cp future_net.tar.gz ../
rm -f future_net.tar.gz
cd ..
rm -rf code