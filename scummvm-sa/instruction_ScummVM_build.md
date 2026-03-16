Link : https://portmaster.games/build-environments.html#2-wsl2-chroot

2. WSL2 chroot 
For more information, visit the GitHub Repository .

Build Environment 

Instructions to set it up: 

Install required packages on Ubuntu 20.04 LTS WSL 2:
sudo apt install build-essential binfmt-support daemonize libarchive-tools qemu-system qemu-user qemu-user-static gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

(Download 20.04 Focal server-cloudimg-arm64-wsl.rootfs.tar.gz from Ubuntu Cloud image. Ubuntu Cloud Image)
wget https://cloud-images.ubuntu.com/releases/focal/release/ubuntu-20.04-server-cloudimg-arm64-root.tar.xz

Extract the tarball in a folder:

mkdir folder
(sudo bsdtar -xpf ubuntu-20.04-server-cloudimg-arm64-wsl.rootfs.tar.gz -C folder)
sudo bsdtar -xpf ubuntu-20.04-server-cloudimg-arm64-root.tar.xz -C folder

Copy qemu static binary into that folder:
sudo cp /usr/bin/qemu-aarch64-static folder/usr/bin

Start systemd with daemonize:
sudo daemonize /usr/bin/unshare -fp --mount-proc /lib/systemd/systemd --system-unit=basic.target

Check if AARCH64 binfmt entry is present:
ls /proc/sys/fs/binfmt_misc/

Mount and chroot into the environment:
sudo mount -o bind /proc folder/proc
sudo mount -o bind /dev folder/dev
sudo chroot folder qemu-aarch64-static /bin/bash

In the chroot, delete /etc/resolv.conf file and write a name server to it.
rm /etc/resolv.conf
echo "nameserver 8.8.8.8" > /etc/resolv.conf
Exit chroot mkdir -p folder/tmp/.X11-unix

Create chroot.sh:

#!/bin/bash

sudo daemonize /usr/bin/unshare -fp --mount-proc /lib/systemd/systemd --system-unit=basic.target

sudo mount -o bind /proc folder/proc
sudo mount -o bind /dev folder/dev
sudo mount -o bind /tmp/.X11-unix folder/tmp/.X11-unix
xhost + local:
sudo chroot folder qemu-aarch64-static /bin/bash

Make the chroot.sh executable:
chmod +x chroot.sh

Chroot into the new environment:
sudo ./chroot.sh

Update & Upgrade the chroot:
apt-get update && apt-get upgrade 

Helpful development tools & libraries to have in the chroot:
apt-get install --no-install-recommends build-essential git wget libdrm-dev python3 python3-pip python3-setuptools python3-wheel ninja-build libopenal-dev premake4 autoconf libevdev-dev ffmpeg libboost-tools-dev magics++ libboost-thread-dev libboost-all-dev pkg-config zlib1g-dev libsdl-mixer1.2-dev libsdl1.2-dev libsdl-gfx1.2-dev libsdl2-mixer-dev clang cmake cmake-data libarchive13 libcurl4 libfreetype6-dev librhash0 libuv1 mercurial mercurial-common libgbm-dev libsdl-image1.2-dev

Install custom SDL2 Libraries for better compatibility:
rm /usr/lib/aarch64-linux-gnu/libSDL2.* 
rm -rf /usr/lib/aarch64-linux-gnu/libSDL2-2.0.so*

wget https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.26.2.tar.gz
./configure --prefix=/usr
make -j$(nproc)
make install

/sbin/ldconfig

For ScummVM build, additional packages needed

apt-get install -y liba52-0.7.4-dev libjpeg62-turbo-dev libfaad-dev libsdl2-net-dev libspeechd-dev libfribidi-dev libglew-dev
apt-get install -y libjpeg-turbo8-dev
apt-get install -y libtheora-dev libcurl4-openssl-dev libmpeg2-4-dev libgif-dev

git clone --recursive https://github.com/FluidSynth/fluidsynth.git
cd fluidsynth
git checkout v2.3.4
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DLIB_SUFFIX="" ..
make -j$(nproc)
make install
cd ..

git clone -b branch-2026-1-0 --recurse-submodules -j"$(nproc)" https://github.com/scummvm/scummvm.git
cd scummvm
patch -p1 < ../scummvm-000-cacert-env.patch
./configure --backend=sdl --enable-optimizations --opengl-mode=gles2 --enable-vkeybd --disable-debug --enable-release --enable-fluidsynth --enable-all-engines
make -j$(nproc)
strip scummvm
apt update && apt install p7zip-full -y
7z a -t7z -m0=lzma2 -mx=9 -md=128m -mfb=273 -ms=on scummvm.7z scummvm

mkdir OUT_DIR
mkdir -p ./OUT_DIR/LICENSES ./OUT_DIR/Theme ./OUT_DIR/Extra
mkdir -p ./OUT_DIR/Extra/shaders
cp -f LICENSES/* ./OUT_DIR/LICENSES/
cp -f dists/soundfonts/COPYRIGHT.Roland_SC-55 ./OUT_DIR/LICENSES/
cp -f gui/themes/*.dat gui/themes/*.zip ./OUT_DIR/Theme/
cp -f dists/networking/wwwroot.zip ./OUT_DIR/Theme/
cp -f -r dists/engine-data/* ./OUT_DIR/Extra/
rm -rf ./OUT_DIR/Extra/patches
rm -rf ./OUT_DIR/Extra/testbed-audiocd-files
rm -f ./OUT_DIR/Extra/README
rm -f ./OUT_DIR/Extra/*.mk
rm -f ./OUT_DIR/Extra/*.sh
cp -f backends/vkeybd/packs/vkeybd_default.zip ./OUT_DIR/Extra/
cp -f backends/vkeybd/packs/vkeybd_small.zip ./OUT_DIR/Extra/
cp -f dists/soundfonts/Roland_SC-55.sf2 ./OUT_DIR/Extra/
find engines/ -type f \( -name "*.fragment" -o -name "*.vertex" \) -exec cp -f {} "./OUT_DIR/Extra/shaders/" \;