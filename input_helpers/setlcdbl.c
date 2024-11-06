#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    int fd;
    unsigned long arg[3];

    if (argc != 2) {
        fprintf(stderr, "wrong number of arguments\n");
        fprintf(stderr, "usage: setlcdbl brightness\n");
        exit(0);
    }

    fd = open("/dev/disp", O_RDWR);

    arg[0] = 0;
    arg[1] = atoi(argv[1]);
    arg[2] = 0;

    ioctl(fd, _IOC(0, 0x01, 0x02, 0x00), arg);

    close(fd);

    exit(0);
}