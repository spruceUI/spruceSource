#!/bin/bash

mkdir /build
cd /build
wget http://ftp.gnu.org/gnu/libc/glibc-2.38.tar.gz
tar -xvzf glibc-2.38.tar.gz
mkdir glibc-build
cd glibc-build

export SYSROOT=/usr/arm-linux-gnueabihf  
export CFLAGS="-O2"
export CXXFLAGS="-O2"

../glibc-2.38/configure --prefix=/home/arm-glibc-2.38 --sharedstatedir=/home/arm-glibc-2.38/lib --host=arm-linux-gnueabihf --build=x86_64-unknown-linux-gnu --with-headers= /usr/arm-linux-gnueabihf/include --disable-multilib libc_cv_forced_unwind=yes libc_cv_c_cleanup=yes CFLAGS="-O2" CXXFLAGS="-O2"

make -j$(nproc)
make install