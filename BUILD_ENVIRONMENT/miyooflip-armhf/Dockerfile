FROM ubuntu:22.04

# Install required tools
RUN apt-get update && apt-get install -y \
    build-essential \
    crossbuild-essential-armhf \
    wget \
    curl \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    git \
    python3 \
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
	pkg-config
	
RUN sudo debootstrap --arch=armhf --foreign buster ./arm-rootfs http://deb.debian.org/debian

COPY glibc_236_build.sh /glibc_236_build.sh
RUN chmod 777 /glibc_236_build.sh
RUN /glibc_236_build.sh

COPY x11-setup.sh /x11-setup.sh
COPY x11-build.sh /x11-build.sh
COPY x11-post.sh /x11-post.sh
RUN chmod 777 /x11-setup.sh 
RUN chmod 777 /x11-build.sh
RUN chmod 777 /x11-post.sh

