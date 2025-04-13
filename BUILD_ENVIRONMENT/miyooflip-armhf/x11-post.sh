#!/bin/bash

# Set the SYSROOT environment variable (you may want to modify this based on your actual setup)
SYSROOT="/home/arm-glibc-2.36"
export PKG_CONFIG_PATH=/home/arm-glibc-2.36/lib/pkgconfig:/home/arm-glibc-2.36/share/pkgconfig

# List of tar.gz URLs
urls=(
    "https://www.x.org/archive/individual/proto/renderproto-0.11.1.tar.gz"
    "https://www.x.org/archive/individual/lib/libXrender-0.9.10.tar.gz"
    "https://www.x.org/archive/individual/proto/fixesproto-5.0.tar.gz"
    "https://www.x.org/archive/individual/lib/libXfixes-5.0.3.tar.gz"
    "https://www.x.org/archive/individual/lib/libXcursor-1.2.0.tar.gz"
    "https://www.x.org/archive/individual/lib/libXext-1.3.4.tar.gz"
    "https://www.x.org/archive/individual/proto/xineramaproto-1.2.1.tar.gz"
    "https://www.x.org/archive/individual/lib/libXinerama-1.1.4.tar.gz"
	"https://www.x.org/archive/individual/proto/randrproto-1.5.0.tar.gz"
    "https://www.x.org/archive/individual/lib/libXrandr-1.5.2.tar.gz"
	"https://xorg.freedesktop.org/archive/individual/proto/scrnsaverproto-1.2.2.tar.gz"
    "https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/libxss/1:1.2.3-1build2/libxss_1.2.3.orig.tar.gz"
	"https://xorg.freedesktop.org/archive/individual/proto/xf86vidmodeproto-2.3.1.tar.gz"
    "https://www.x.org/archive/individual/lib/libXxf86vm-1.1.4.tar.gz"
)

# Create the /build/x11 directory if it doesn't exist
mkdir -p /build/x11

# Loop over each URL, download, extract, configure, build, and install
for url in "${urls[@]}"; do
    # Extract the filename from the URL
    filename=$(basename "$url")
    
    # Define the directory name based on the tar.gz filename
    dir_name="${filename%.tar.gz}"
    
    if [[ "$filename" == "libxss_1.2.3.orig.tar.gz" ]]; then
        dir_name="libXScrnSaver-1.2.3"
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
        CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ --enable-malloc0returnsnull\
        CFLAGS="-I$SYSROOT/include" LDFLAGS="-L$SYSROOT/lib"  || { echo "Configure failed for $dir_name"; exit 1; }
    
    # Build and install
    echo "Building and installing $dir_name..."
    make
    make install || { echo "Configure failed for $dir_name"; exit 1; }
    
    # Go back to /build/x11 to process the next tar.gz
    cd /build/x11 || exit
done

mkdir x11
cp /home/arm-glibc-2.36/lib/libX11.so.6.4.0 /x11/libX11.so.6
cp /home/arm-glibc-2.36/lib/libXcursor.so.1.0.2 /x11/libXcursor.so.1
cp /home/arm-glibc-2.36/lib/libXinerama.so.1.0.0 /x11/libXinerama.so.1
cp /home/arm-glibc-2.36/lib/libXrandr.so.2.2.0 /x11/libXrandr.so.2
cp /home/arm-glibc-2.36/lib/libXss.so.1.0.0 /x11/libXss.so.1
cp /home/arm-glibc-2.36/lib/libXxf86vm.so.1.0.0 /x11/libXxf86vm.so.1
cp /home/arm-glibc-2.36/lib/libXrender.so.1.3.0 /x11/libXrender.so.1
cp /home/arm-glibc-2.36/lib/libXfixes.so.3.1.0 /x11/libXfixes.so.3






echo "All dependencies completed!"
