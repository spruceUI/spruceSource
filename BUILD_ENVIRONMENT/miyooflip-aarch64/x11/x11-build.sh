#!/bin/bash
echo Starting X11!

url="https://www.x.org/releases/individual/lib/libX11-1.8.7.tar.gz"
# Extract the filename from the URL
filename=$(basename "$url")
    
# Define the directory name based on the tar.gz filename
dir_name="${filename%.tar.gz}"
# Download the tar.gz file to /build/x11
echo "Downloading $url..."
curl -L "$url" -o "/build/x11/$filename"
    
# Extract the tar.gz file
echo "Extracting $filename..."
tar -xf "/build/x11/$filename" -C /build/x11
    
# Change into the extracted directory
cd "/build/x11/$dir_name" || exit

./configure \
   --disable-tests \
   --enable-shared \
   --sharedstatedir=//home/arm-glibc-2.38/lib \
   --host=arm-linux-gnueabihf \
   --build=x86_64-unknown-linux-gnu \
   --prefix=$SYSROOT \
   --enable-cross-compiling \
   --disable-memcheck \
   --enable-malloc0returnsnull\
   CC=arm-linux-gnueabihf-gcc \
   CXX=arm-linux-gnueabihf-g++  \
   CFLAGS="-I$SYSROOT/include" \
   LDFLAGS="-L$SYSROOT/lib"  || { echo "Configure failed for $dir_name"; exit 1; }

./configure --host=x86_64-unknown-linux-gnu --enable-shared --sharedstatedir=//home/arm-glibc-2.38/lib \
        --host=arm-linux-gnueabihf --build=x86_64-unknown-linux-gnu --prefix=$SYSROOT \
        CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ --enable-malloc0returnsnull \
        CFLAGS="-I$SYSROOT/include" LDFLAGS="-L$SYSROOT/lib"  || { echo "Configure failed for $dir_name"; exit 1; }

make install  || { echo "Configure failed for $dir_name"; exit 1; }