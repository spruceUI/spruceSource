FROM ubuntu:22.04

COPY sources.list /etc/apt/sources.list
COPY arm64.list /etc/apt/sources.list.d/arm64.list
RUN dpkg --add-architecture arm64

# Install required tools
RUN apt-get update && apt-get install -y \
    build-essential \
    crossbuild-essential-arm64 \
    wget \
    curl \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    git \
    python3 \
	python3-pip \
    sudo \
    texinfo \
    bison \
    gawk \
    libmpc-dev \
    libmpfr-dev \
    libgmp-dev \
    libssl-dev \
    flex \
    libncurses-dev \
    file \
	libtool \
	libtool-bin \
	build-essential \
	gawk \
	bison \
	texinfo \
	libgmp-dev \
	libmpc-dev \
	libmpfr-dev \
	flex \
	debootstrap \
	vim \
	pkg-config \
	cmake \
	libcurl4-openssl-dev:arm64 \
	libgl1-mesa-dev:arm64 \
	libgles2-mesa-dev:arm64 \
	libegl1-mesa-dev \
	libgles2-mesa-dev \
	libpng-dev:arm64 \
	libsdl2-dev:arm64
	
RUN sudo debootstrap --arch=arm64 --foreign buster ./arm-rootfs http://deb.debian.org/debian

COPY glibc_238_build.sh /glibc_238_build.sh
RUN chmod 777 /glibc_238_build.sh
RUN /glibc_238_build.sh

COPY x11-setup.sh /x11-setup.sh
COPY x11-build.sh /x11-build.sh
COPY x11-post.sh /x11-post.sh
RUN chmod 777 /x11-setup.sh 
RUN chmod 777 /x11-build.sh
RUN chmod 777 /x11-post.sh

COPY libFAudio.so.0 /libFAudio.so.0
