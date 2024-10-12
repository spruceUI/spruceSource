# Use Ubuntu 16.04 as the base image
FROM ubuntu:16.04

# Set non-interactive mode for apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies for building Samba
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    wget \
    pkg-config \
    autoconf \
    automake \
    libtool \
    python3 \
    git \
    bash \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    libc6-dev-armhf-cross \
    && rm -rf /var/lib/apt/lists/*

# Set the cross-compilation prefix
ENV CROSS_COMPILE="arm-linux-gnueabihf-"

# Set the working directory
WORKDIR /src

# Download and extract the specific Samba version 3.6.23
RUN wget --no-check-certificate http://samba.org/samba/ftp/stable/samba-3.6.23.tar.gz && \
    tar -xzf samba-3.6.23.tar.gz && \
    rm samba-3.6.23.tar.gz

# Set up the environment and configure the build for ARM
RUN cd samba-3.6.23/source3 && \
    ./autogen.sh && \
    ./configure \
        --without-krb5 \
        --without-ldap \
        --without-ads \
        --disable-cups \
        --enable-swat=no \
        --with-winbind=no \
        --host=arm-linux-gnueabihf \
        --prefix=/usr/arm-linux-gnueabihf \
        --with-configdir=/mnt/SDCARD/.tmp_update/etc \
        samba_cv_CC_NEGATIVE_ENUM_VALUES=yes \
        libreplace_cv_HAVE_GETADDRINFO=no \
        ac_cv_file__proc_sys_kernel_core_pattern=yes 

# Build and install Samba using the ARM cross-compiler
RUN cd samba-3.6.23/source3 && \
    make && \
    make install DESTDIR=/home/export/rootfs

# Copy the compiled binaries out of the Docker container
RUN mkdir -p /home/export/rootfs/usr && \
    cp -r /usr/arm-linux-gnueabihf/* /home/export/rootfs/usr/

# Set the entrypoint to keep the container running
ENTRYPOINT ["/bin/bash"]

