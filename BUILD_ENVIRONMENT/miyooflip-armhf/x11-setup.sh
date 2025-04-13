#!/bin/bash

# Set the SYSROOT environment variable (you may want to modify this based on your actual setup)
SYSROOT="/home/arm-glibc-2.36"
export PKG_CONFIG_PATH=/home/arm-glibc-2.36/lib/pkgconfig:/home/arm-glibc-2.36/share/pkgconfig

# List of tar.gz URLs
urls=(
    "https://www.x.org/releases/individual/proto/xproto-7.0.31.tar.gz"
    "https://xorg.freedesktop.org/archive/individual/proto/xcb-proto-1.15.2.tar.gz"
    "https://xorg.freedesktop.org/archive/individual/lib/libXau-1.0.11.tar.gz"
    "https://xorg.freedesktop.org/archive/individual/proto/xextproto-7.3.0.tar.gz"
    "https://www.x.org/releases/individual/xcb/libxcb-1.15.tar.gz"
    "https://deb.debian.org/debian/pool/main/x/xtrans/xtrans_1.4.0.orig.tar.gz"
    "https://xorg.freedesktop.org/archive/individual/proto/kbproto-1.0.7.tar.gz"
    "https://xorg.freedesktop.org/archive/individual/proto/inputproto-2.3.2.tar.gz"
)

# Create the /build/x11 directory if it doesn't exist
mkdir -p /build/x11

# Loop over each URL, download, extract, configure, build, and install
for url in "${urls[@]}"; do
    # Extract the filename from the URL
    filename=$(basename "$url")
    
    # Define the directory name based on the tar.gz filename
    dir_name="${filename%.tar.gz}"
    
	if [[ "$filename" == "xtrans_1.4.0.orig.tar.gz" ]]; then
        dir_name="xtrans-1.4.0"
    fi
	
    # Download the tar.gz file to /build/x11
    echo "Downloading $url..."
    curl -L "$url" -o "/build/x11/$filename"
    
    # Extract the tar.gz file
    echo "Extracting $filename..."
    tar -xf "/build/x11/$filename" -C /build/x11
    
    # Change into the extracted directory
    cd "/build/x11/$dir_name" || exit
    
    # Run configure with the specified parameters
    echo "Configuring $dir_name..."
    ./configure --host=x86_64-unknown-linux-gnu --enable-shared --sharedstatedir=//home/arm-glibc-2.36/lib \
        --host=arm-linux-gnueabihf --build=x86_64-unknown-linux-gnu --prefix=$SYSROOT \
        CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ \
        CFLAGS="-I$SYSROOT/include" LDFLAGS="-L$SYSROOT/lib"  || { echo "Configure failed for $dir_name"; exit 1; }
    
    # Build and install
    echo "Building and installing $dir_name..."
    make
    make install || { echo "Configure failed for $dir_name"; exit 1; }
    
    # Go back to /build/x11 to process the next tar.gz
    cd /build/x11 || exit
done

echo "All dependencies completed!"
