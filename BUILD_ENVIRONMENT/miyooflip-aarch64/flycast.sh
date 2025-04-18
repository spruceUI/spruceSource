#!/bin/bash
cd build
git clone https://github.com/libretro/libretro-super.git
cd libretro-super

git clone https://github.com/flyinghead/flycast.git
cd flycast
git submodule update --init --recursive

mkdir build-aarch64 
cd build-aarch64

SYSROOT="/home/arm-glibc-2.38"

apt download libgl-dev:arm64
dpkg -x libgl-dev*.deb /home/arm-glibc-2.38/

# point /home/arm-glibc-2.38/include/gnu to stubshard

cmake .. \
  -DCMAKE_SYSTEM_NAME=Linux \
  -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
  -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBRETRO=Y \
  -DCMAKE_TOOLCHAIN_FILE=/aarch64-toolchain.cmake \
  -DCMAKE_SYSROOT=/home/arm-glibc-2.38 \
  -D_FILE_OFFSET_BITS=64 \
  -DCMAKE_C_FLAGS="--sysroot=/home/arm-glibc-2.38 -I/home/arm-glibc-2.38/include -D_FILE_OFFSET_BITS=64 -fpermissive -DSIZEOF_OFF_T=8" \
  -DCMAKE_CXX_FLAGS="--sysroot=/home/arm-glibc-2.38 -I/home/arm-glibc-2.38/include -D_FILE_OFFSET_BITS=64 -fpermissive -DSIZEOF_OFF_T=8"
  
make -j$(nproc)

mkdir -p /retroarch-cores
cp flycast_libretro.so /retroarch-cores/flycast_libretro.so
