#!/bin/bash

cd /build
git clone https://github.com/chrisj951/gl4es.git
cd gl4es
mkdir build
cd build

export PKG_CONFIG_SYSROOT_DIR=/home/arm-glibc-2.38 
export PKG_CONFIG_PATH=/home/arm-glibc-2.38/lib/pkgconfig/:/home/arm-glibc-2.38/usr/share/pkgconfig:/home/arm-glibc-2.38/usr/lib/arm-linux-gnueabihf/pkgconfig

mkdir -p /home/arm-glibc-2.38/usr
ln -s /home/arm-glibc-2.38/include /home/arm-glibc-2.38/usr/include

cmake .. \
  -DNOX11=ON \
  -DGLX_STUBS=ON \
  -DEGL_WRAPPER=ON \
  -DGBM=ON \
  -DCMAKE_SYSROOT=/home/arm-glibc-2.38 \
  -DCMAKE_FIND_ROOT_PATH=/home/arm-glibc-2.38 \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
  -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
  
cd /build
git clone https://github.com/chrisj951/box86.git
mkdir build
cd build
cmake .. \
  -DNOX11=ON \
  -DGLX_STUBS=ON \
  -DEGL_WRAPPER=ON \
  -DGBM=ON \
  -DCMAKE_SYSROOT=/home/arm-glibc-2.38 \
  -DCMAKE_FIND_ROOT_PATH=/home/arm-glibc-2.38 \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
  -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
make