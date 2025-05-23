TOOLCHAIN_ARCH=`uname -m`
if [ "$TOOLCHAIN_ARCH" = "aarch64" ]; then
	export CROSS_COMPILE=/usr/bin/aarch64-linux-gnu-   
	export PREFIX=/usr 
else
	export PATH="/opt/aarch64-none-linux-gnu/aarch64-none-linux-gnu/bin:${PATH}:/opt/aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/usr/bin"
	export CROSS_COMPILE=/opt/aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
	export PREFIX=/opt/aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/usr
fi
export UNION_PLATFORM=my355