# rumble - evdev force feedback helper
#
# Freestanding: no libc, raw syscalls, static. That is deliberate. spruce ships
# one binary to devices with quite different glibc versions, and a build box
# with no aarch64 sysroot can still produce it - clang cross compiles without
# one because nothing outside the kernel ABI is referenced.
#
#   make                 build with clang for aarch64 (needs lld)
#   make CC=... CROSS_COMPILE=...   build with a cross gcc instead

TARGET  = rumble
ARCH   ?= aarch64
CC     ?= clang

ifeq ($(origin CC),default)
CC = clang
endif

CLANG_FLAGS = --target=$(ARCH)-linux-gnu -fuse-ld=lld
GCC_FLAGS   =

# -nostdlib/-nostartfiles: we supply _start ourselves and call no library code.
# -static: nothing to link against at runtime, so no interpreter is recorded.
FLAGS = -Os -static -nostdlib -nostartfiles -ffreestanding \
        -fno-stack-protector -fno-builtin -Wall -Wextra -s

all:
ifeq ($(CC),clang)
	$(CC) $(CLANG_FLAGS) $(FLAGS) -o $(TARGET) $(TARGET).c
else
	$(CROSS_COMPILE)$(CC) $(GCC_FLAGS) $(FLAGS) -o $(TARGET) $(TARGET).c
endif

clean:
	rm -f $(TARGET)
