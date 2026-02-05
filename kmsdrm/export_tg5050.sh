#!/bin/bash

# 1. Base SDK and Toolchain paths (use your sdk_path)
export SDK_PATH="/home/ark/sdk_tg5050_linux_v1.0.0"
export TOOLCHAIN_BIN="$SDK_PATH/host/opt/ext-toolchain/bin"
export CROSS_COMPILE="aarch64-none-linux-gnu-"

# 2. Verified Sysroot path
# Contains: /usr/lib/libc_nonshared.a and /usr/include/libdrm
export SYSROOT="$SDK_PATH/host/aarch64-buildroot-linux-gnu/sysroot"

# 3. Update PATH to include the cross-compiler
export PATH="$TOOLCHAIN_BIN:$PATH"

# 4. Standard Compiler variables
export CC="${CROSS_COMPILE}gcc"
export CXX="${CROSS_COMPILE}g++"

# 5. Configuration for CMake and Pkg-Config
# Using verified /usr/lib/pkgconfig directory
export PKG_CONFIG_PATH="$SYSROOT/usr/lib/pkgconfig"
export PKG_CONFIG_LIBDIR="$SYSROOT/usr/lib/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="$SYSROOT"

echo "-------------------------------------------------------"
echo "✅ TG5050 (A523) Verified Environment Loaded"
echo "SDK_PATH: $SDK_PATH"
echo "SYSROOT:  $SYSROOT"
echo "CC:       $CC"
echo "-------------------------------------------------------"