#!/bin/bash

# Static libcurl crosscompiler #########################################################################################

# This is script to cross-compiles a **fully static** curl library.
# It builds WolfSSL and C-Ares, then LibCurl - therein supporting most SSL protocols including TSL 1.3.

# Helper variables, leave as is

export BASEDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export BUILDROOT=$BASEDIR/build
export SYSROOT=${BUILDROOT}/sysroot

# Configure (cross) compiler root and prefix - adapt these to your crosscompiler

export INSTALL_PACKAGES=false

export CROSS_ROOT=/opt/a30
export COMPILER_PREFIX=arm-linux-

#export BUILD=arm-linux-gnueabihf
#export ARCH=arm
export HOST=x86_64 #arm-a30-linux-gnueabihf

#export CROSS_FLAGS="-mcpu=cortex-a8 -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8"
#export CROSS_FLAGS="-march=armv8-a	-mtune=cortex-a53 -mcpu=cortex-a53"
CROSS_FLAGS=""
# Do not edit past this line ###########################################################################################

export PATH=$CROSS_ROOT/bin:/bin:/usr/bin

export BASEDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

export CPPFLAGS="-I${CROSS_ROOT}/include"
export AR=${CROSS_ROOT}/bin/${COMPILER_PREFIX}ar
export AS=${CROSS_ROOT}/bin/${COMPILER_PREFIX}as
export LD=${CROSS_ROOT}/bin/${COMPILER_PREFIX}ld
export RANLIB=${CROSS_ROOT}/bin/${COMPILER_PREFIX}ranlib
export CC=${CROSS_ROOT}/bin/${COMPILER_PREFIX}gcc
export NM=${CROSS_ROOT}/bin/${COMPILER_PREFIX}nm

set -eu
echo $CC
# Install
install_packages() {
  apt-get update && apt-get upgrade -yy

  local packages=(

    # Required build tools
    make
    cmake
    autoconf
    automake
    pkg-config
    libtool
    flex
    bison
    gettext

    # Utilities required
    curl
    ca-certificates
    xz-utils

    # For debugging
    less
    silversearcher-ag
    tree
    vim
  )

  apt-get install -yy --no-install-recommends "${packages[@]}"
}

# Download helper function
fetch() {
  local url fileopt

  if [[ $# -eq 2 ]]; then
    fileopt="-o $1"
    url="$2"
  elif [[ $# -eq 1 ]]; then
    fileopt="-O"
    url="$1"
  else
    echo "Bad args"
    exit 1
  fi

  curl -L '-#' $fileopt $url
}

fetch_deps() {
  cd $BUILDROOT/srcs

  echo "Fetching sources..."
  #fetch https://github.com/wolfSSL/wolfssl/archive/v4.6.0-stable.tar.gz
  fetch https://www.openssl.org/source/openssl-3.3.2.tar.gz
  #fetch https://www.openssl.org/source/openssl-1.1.1h.tar.gz
  fetch https://github.com/c-ares/c-ares/releases/download/cares-1_17_2/c-ares-1.17.2.tar.gz
  fetch https://curl.se/download/curl-8.10.1.tar.xz
  #fetch https://curl.se/download/curl-7.75.0.tar.xz
  #fetch https://curl.se/download/curl-7.88.1.tar.xz
  #fetch https://curl.se/download/curl-7.73.0.tar.xz
}

build_wolfssl() {
  cd $BUILDROOT/build
  tar -xf $BUILDROOT/srcs/v4.6.0* && cd wolf*

  ./autogen.sh

  ./configure \
    --prefix=$SYSROOT \
    --enable-tls13 \
    --exec-prefix=$SYSROOT \
    --host=$HOST \
    --disable-shared \
    CFLAGS="-fPIC ${CROSS_FLAGS} -fomit-frame-pointer -fno-PIC -DTIME_T_NOT_64BIT"

  make -j4 AR='${COMPILER_PREFIX}ar r ' RANLIB=${COMPILER_PREFIX}ranlib
  make install
}

build_openssl() {
    cd $BUILDROOT/build
    tar -xf $BUILDROOT/srcs/openssl* && cd *openssl*

    ./Configure linux-generic32 no-shared no-asm\
    --prefix=$SYSROOT \
    --openssldir=$SYSROOT \
    -static
    make -j6
    make install

}

build_cares() {
  cd $BUILDROOT/build
  tar -xf $BUILDROOT/srcs/c-ares* && cd *c-ares*
  ./configure CFLAGS="-fPIC ${CROSS_FLAGS}" --host=$HOST --prefix=$SYSROOT --with-pic --enable-static --disable-shared
  make -j4
  make install
}

build_curl_openssl() {
  cd $BUILDROOT/build
  tar -xf $BUILDROOT/srcs/curl-*
  cd curl-*
  ./configure --host=$HOST --prefix=$SYSROOT \
    --without-libpsl \
    --disable-shared \
    --enable-static \
    --disable-netrc \
    --disable-ipv6 \
    --disable-verbose \
    --disable-versioned-symbols \
    --disable-ntlm-wb \
    --disable-proxy \
    --disable-manual \
    --disable-crypto-auth \
    --disable-ldap --disable-sspi --without-librtmp --disable-ftp --disable-file --disable-dict \
    --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-rtsp \
    --disable-ldap --disable-ipv6 --disable-unix-sockets \
    --disable-gopher --disable-smb --without-libidn \
    --disable-wolfssl \
    --with-openssl=$SYSROOT \
    --without-zlib \
    --enable-ares=$SYSROOT \
    CFLAGS="-fPIC ${CROSS_FLAGS}" \
    LIBS="-lm" \
    PKG_CONFIG_PATH="$SYSROOT/lib/pkgconfig"
  #LIBS="-ldl"
  make -j6 curl_LDFLAGS=-all-static PROCESSOR=ARM
  make install
  # -L$SYSROOT/lib -lcrypto -L$SYSROOT/lib -lssl -Wl,--rpath=/opt/a30/arm-a30-linux-gnueabihf/sysroot/lib/ -Wl,--dynamic-linker=/opt/a30/arm-a30-linux-gnueabihf/sysroot/lib/ld-linux-armhf.so.3
}

build_curl() {
  cd $BUILDROOT/build
  tar -xf $BUILDROOT/srcs/curl-*
  cd curl-*
  ./configure --host=$HOST --prefix=$SYSROOT \
    --disable-pthreads \
    --disable-shared \
    --enable-static \
    --disable-netrc \
    --disable-ipv6 \
    --disable-verbose \
    --disable-versioned-symbols \
    --disable-ntlm-wb \
    --disable-proxy \
    --disable-manual \
    --disable-crypto-auth \
    --disable-ldap --disable-sspi --without-librtmp --disable-ftp --disable-file --disable-dict \
    --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-rtsp \
    --disable-ldap --disable-ipv6 --disable-unix-sockets \
    --disable-gopher --disable-smb --without-libidn \
    --without-libpsl \
    --with-wolfssl=$SYSROOT \
    --without-zlib \
    --enable-ares=$SYSROOT \
    CFLAGS="-fPIC ${CROSS_FLAGS}" \
    LIBS="-ldl -lm" \
    PKG_CONFIG_PATH="$SYSROOT/lib/pkgconfig"
  LIBS="-ldl"
  make -j4 curl_LDFLAGS=-all-static -ldl
  make install
}

main() {

  # download and install packages
  if [ "INSTALL_PACKAGES" = true ]; then
    install_packages
  fi

  # clean start
  rm -rf $BASEDIR/build
  mkdir $BASEDIR/build

  mkdir -p $BUILDROOT/srcs $BUILDROOT/build $SYSROOT $SYSROOT/lib
  #cp ./libcrypto.a $SYSROOT/lib/.
  #cp ./libssl.a $SYSROOT/lib/.


  #    install_packages
  fetch_deps

  build_openssl
  build_cares
  #build_wolfssl


  if [ -f $SYSROOT/lib/pkgconfig/wolfssl.pc ]; then
    mv $SYSROOT/lib/pkgconfig/wolfssl.pc $SYSROOT/lib/pkgconfig/wolfssl.pc_
  fi
  if [ -f $SYSROOT/lib/pkgconfig/openssl.pc ]; then
    mv $SYSROOT/lib/pkgconfig/openssl.pc $SYSROOT/lib/pkgconfig/openssl.pc_
  fi

  if [ -f $SYSROOT/lib/pkgconfig/libcares.pc ]; then
    mv $SYSROOT/lib/pkgconfig/libcares.pc $SYSROOT/lib/pkgconfig/libcares.pc_
  fi

  build_curl_openssl
  #build_curl

  # clean
  rm -rf $BASEDIR/build/build
  rm -rf $BASEDIR/build/srcs

}

main "$@"
