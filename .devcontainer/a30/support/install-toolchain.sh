#!/bin/sh

mkdir -p /opt/my282-toolchain
if [ -d /opt/my282-toolchain/usr ]; then
	rm -fr /opt/my282-toolchain/usr
fi
cp -rf ~/buildroot/output/host/usr/ /opt/my282-toolchain/
# this version of buildroot doesn't have relocate-sdk.sh yet so we bring our own
cp ~/relocate-sdk.sh /opt/my282-toolchain/
cp ~/sdk-location /opt/my282-toolchain/
/opt/my282-toolchain/relocate-sdk.sh