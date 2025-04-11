#!/bin/sh

cd /opt/
tar --xz -cvf my282-toolchain.tar.xz my282-toolchain/
mv my282-toolchain.tar.xz ~/workspace/

printf "my282-toolchain.tar.xz can be shared as a blob\nby placing in support before calling 'make shell'\n"