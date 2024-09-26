#!/bin/bash

# Set up environment variables
export CROSS_COMPILE=arm-linux-gnueabihf-
export CC=${CROSS_COMPILE}gcc
export CXX=${CROSS_COMPILE}g++
export CFLAGS="-static -O2"
export CXXFLAGS="-static -O2"
export LDFLAGS="-static -Wl,--strip-all -Wl,--no-dynamic-linker"

# Download and extract curl source
curl_version="7.88.1"
wget https://curl.se/download/curl-${curl_version}.tar.gz
tar xzf curl-${curl_version}.tar.gz
cd curl-${curl_version}

# Configure curl with minimal features
./configure --host=arm-linux-gnueabihf \
    --disable-shared \
    --enable-static \
    --disable-ipv6 \
    --disable-manual \
    --disable-threaded-resolver \
    --disable-verbose \
    --without-ssl \
    --without-zlib \
    --without-libidn2 \
    --without-librtmp \
    --without-nghttp2 \
    --without-brotli \
    --enable-symbol-hiding \
    --without-libpsl \
    --without-libgsasl

# Compile
make LDFLAGS="-all-static -static-libgcc -Wl,--no-dynamic-linker"

# Strip the binary
${CROSS_COMPILE}strip src/curl

echo "Compilation complete. The statically linked curl binary is located at src/curl"
