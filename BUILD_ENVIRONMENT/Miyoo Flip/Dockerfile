# Miyoo FLIP - Docker Build  Environment
# DOCKER BUILD COMMAND: docker build -t flip-dev .
# USE: docker run --rm -it -v $(pwd)/workspace:/workspace flip-dev
#      (this will bind to /workspace in the docker to ./workspace on local fs)
# NOTE: Binaries compiled will be associated with GLIBC 2.34

# Use Ubuntu 22.04 (Jammy Jellyfish) as the base image
FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV WORKSPACE=/workspace
ENV CROSS_COMPILE_PATH=/opt/aarch64-toolchain
ENV TOOLCHAIN_URL="https://github.com/steward-fu/website/releases/download/miyoo-flip/miyoo355_sdk_release.20241121.tgz"
ENV TOOLCHAIN_ARCHIVE="miyoo355_sdk_release.20241121.tgz"
ENV TOOLCHAIN_DIR="miyoo355_sdk_release/host"
ENV SYSROOT="${CROSS_COMPILE_PATH}/${TOOLCHAIN_DIR}/aarch64-buildroot-linux-gnu/sysroot"
ENV BIN_DIR="${CROSS_COMPILE_PATH}/${TOOLCHAIN_DIR}/bin"
ENV INCLUDE_DIR="${CROSS_COMPILE_PATH}/${TOOLCHAIN_DIR}/include"
ENV LIB_DIR="${CROSS_COMPILE_PATH}/${TOOLCHAIN_DIR}/lib"
ENV LIB64_DIR="${CROSS_COMPILE_PATH}/${TOOLCHAIN_DIR}/lib64"
ENV PATH="${BIN_DIR}:${PATH}"
ENV CROSS_COMPILE=aarch64-buildroot-linux-gnu-
ENV CC="${BIN_DIR}/aarch64-buildroot-linux-gnu-gcc"
ENV CXX="${BIN_DIR}/aarch64-buildroot-linux-gnu-g++"
ENV AR="${BIN_DIR}/aarch64-buildroot-linux-gnu-ar"
ENV AS="${BIN_DIR}/aarch64-buildroot-linux-gnu-as"
ENV LD="${BIN_DIR}/aarch64-buildroot-linux-gnu-ld"
ENV RANLIB="${BIN_DIR}/aarch64-buildroot-linux-gnu-ranlib"
ENV STRIP="${BIN_DIR}/aarch64-buildroot-linux-gnu-strip"
ENV PKG_CONFIG_PATH="${SYSROOT}/usr/lib/pkgconfig:${SYSROOT}/lib/pkgconfig"
ENV CFLAGS="--sysroot=${SYSROOT} -I${INCLUDE_DIR}"
ENV CXXFLAGS="--sysroot=${SYSROOT} -I${INCLUDE_DIR}"
ENV LDFLAGS="--sysroot=${SYSROOT} -L${LIB_DIR} -L${LIB64_DIR}"

# Update and install essential packages
RUN apt-get update && apt-get install -y \
    wget \
    tar \
    build-essential \
    gcc \
    g++ \
    make \
    git \
    pkg-config \
    curl \
    libtool \
    automake \
    autoconf \
    python3 \
    python3-pip \ 
    vim \
    nano \
    sudo \
    && apt-get clean

# Download and extract the cross-compilation toolchain
RUN wget ${TOOLCHAIN_URL} -O /tmp/${TOOLCHAIN_ARCHIVE} && \
    mkdir -p ${CROSS_COMPILE_PATH} && \
    tar -xzf /tmp/${TOOLCHAIN_ARCHIVE} -C ${CROSS_COMPILE_PATH} && \
    rm /tmp/${TOOLCHAIN_ARCHIVE}

# Create workspace for user interaction
RUN mkdir -p ${WORKSPACE}
WORKDIR ${WORKSPACE}

# Set up the workspace as a volume
VOLUME [ "${WORKSPACE}" ]

# Start with an interactive shell
CMD ["bash"]
