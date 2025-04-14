#! /bin/sh

SYSROOT_TAR="miyoo355_sdk_release.20241121"
SYSROOT_URL=""https://github.com/steward-fu/website/releases/download/miyoo-flip/$SYSROOT_TAR.tgz""
TOOLCHAIN_NAME="aarch64-linux-gnu"
cd ~
wget $SYSROOT_URL
tar xf $SYSROOT_TAR.tgz
rsync -a --ignore-existing ./usr/ /opt/$TOOLCHAIN_NAME/$TOOLCHAIN_NAME/libc/usr/
rm -rf ./usr $SYSROOT_TAR.tgz

TOOLCHAIN_ARCH=`uname -m`
if [ "$TOOLCHAIN_ARCH" = "aarch64" ]; then
	echo "toolchain is aarch64, exiting"
	exit
fi

echo "building toolchain"

TOOLCHAIN_TAR="gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu"
TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/$TOOLCHAIN_TAR.tar.xz"

wget $TOOLCHAIN_URL

tar xf $TOOLCHAIN_TAR.tar.xz -C /opt
mv /opt/$TOOLCHAIN_TAR /opt/$TOOLCHAIN_NAME
rm $TOOLCHAIN_TAR.tar.xz
