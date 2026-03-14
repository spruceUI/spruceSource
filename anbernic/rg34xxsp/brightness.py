import os
import fcntl
import struct
import sys

DEV = "/dev/disp"
IOCTL_SET_BRIGHTNESS = 0x102

BRIGHT_TABLE = [5,10,20,50,70,140,200,255]

def set_brightness(level):
    level = max(0, min(7, int(level)))
    val = BRIGHT_TABLE[level]

    # 4 unsigned long values (ARM64 = 8 bytes each)
    args = struct.pack("QQQQ", 0, val, 0, 0)

    fd = os.open(DEV, os.O_RDWR)
    fcntl.ioctl(fd, IOCTL_SET_BRIGHTNESS, args)
    os.close(fd)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: brightness.py <0-7>")
        exit(1)

    set_brightness(sys.argv[1])