#! /bin/sh

TOOLCHAIN_ARCH=`uname -m`
if [ "$TOOLCHAIN_ARCH" = "aarch64" ]; then
	echo "toolchain is aarch64, exiting"
	exit
fi

echo "building toolchain"

SYSROOT_TAR="miyoo355_sdk_release.20241121"
TOOLCHAIN_NAME="aarch64-linux-gnu"
TOOLCHAIN_TAR="gcc-arm-8.3-2019.02-x86_64-aarch64-linux-gnu"

TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu-a/8.3-2019.02/$TOOLCHAIN_TAR.tar.xz"
SYSROOT_URL=""https://github.com/steward-fu/website/releases/download/miyoo-flip/$SYSROOT_TAR.tgz""

cd ~

wget $TOOLCHAIN_URL
wget $SYSROOT_URL

tar xf $TOOLCHAIN_TAR.tar.xz -C /opt
mv /opt/$TOOLCHAIN_TAR /opt/$TOOLCHAIN_NAME
rm $TOOLCHAIN_TAR.tar.xz

tar xf $SYSROOT_TAR.tgz
rsync -a --ignore-existing ./usr/ /opt/$TOOLCHAIN_NAME/$TOOLCHAIN_NAME/libc/usr/
rm -rf ./usr $SYSROOT_TAR.tgz