# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04
# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive
# Update and install necessary packages
RUN apt-get update && apt-get install -y \
    wget \
    curl \
    build-essential \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    vim \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
# Set up environment variables for cross-compilation
ENV CROSS_COMPILE=arm-linux-gnueabihf-
ENV CC=${CROSS_COMPILE}gcc
ENV CXX=${CROSS_COMPILE}g++
# Set the working directory
WORKDIR /work
# Copy the curl compilation script into the container
COPY compile_static_curl.sh /work/
# Make the script executable
RUN chmod +x /work/compile_static_curl.sh
# Run the compilation script
CMD ["/work/compile_static_curl.sh"]