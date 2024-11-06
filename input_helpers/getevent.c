// Author: oscar (https://github.com/oscarkcau)
//
// CLI tool that reads input events from specified input device and prints to stdout.
//
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <poll.h>
#include <unistd.h>
#include <stdbool.h>

void print_usage();

void print_usage() {
    fprintf(stdout, "usage: getevent input_device [-exclusive]\n");
    fprintf(stdout, "Read input from input_device and print to stdout.\n\n");
    fprintf(stdout, "-exclusive\topen input device exclusively, other process cannot read input from the same device.\n");
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    struct pollfd pfd;
    struct input_event event;

    // check number of arguments
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "wrong number of arguments\n");
        print_usage();
        return -1;
    }

    // initial file open flag
    int open_flag = O_RDONLY;
    bool is_exclusive = false;
    if (argc == 3) {
        if (strcmp(argv[2], "-exclusive") == 0) {
            open_flag = O_RDWR;
            is_exclusive = true;
        }
        else {
            fprintf(stderr, "invalid argument %s\n", argv[2]);
            print_usage();
            return -1;
        }
    }

    // open input device
    fd = open(argv[1], open_flag );
    if(fd <= 0) {
        fprintf(stderr, "could not open %s\n", argv[1]);
        print_usage();
        return -1;
    }

    // "grab" the device so other process cannot access it
    if (is_exclusive) {
        sleep(1);
        ioctl(fd, EVIOCGRAB, 1);
    }

    pfd.fd = fd;
    pfd.events = POLLIN;

    while(1) {

        // poll one event at a time
        poll(&pfd, 1, -1);

        if(pfd.revents & POLLIN) {

            // read the event
            ret = read(pfd.fd, &event, sizeof(event));

            // ignore on reading error
            if(ret < (int)sizeof(event)) {
                fprintf(stderr, "could not get event\n");
                continue;
            }

            // skip event separator
            if (event.type == 0 && event.code == 0 && event.value == 0)
                continue;

            // print event to stdout
            printf("key %d %d %d\n", event.type, event.code, event.value);

            // flush after each printing
            // avoid buffering issue in piping and redirection
            fflush(stdout);
        }
    }

    return 0;
}