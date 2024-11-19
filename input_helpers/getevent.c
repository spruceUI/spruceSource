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
#include <stdlib.h>
#include <signal.h>

void print_usage();
void print_error_usage_exit(const char *err);
void print_error_arg_usage_exit(const char *err, void* arg);

void print_usage() {
    fprintf(stdout, "usage: getevent input_device [-exclusive] [-pid PID]\n");
    fprintf(stdout, "Read input from input_device and print to stdout.\n\n");
    fprintf(stdout, "-exclusive\topen input device exclusively, other process cannot read input from the same device.\n");
    fprintf(stdout, "-pid\tterminate after process ID, PID dies.\n");
}

void print_error_usage_exit(const char *err) {
    fprintf(stderr, err);
    fprintf(stderr, "\n");
    print_usage();
    exit(-1);
}

void print_error_arg_usage_exit(const char *err, void* arg) {
    fprintf(stderr, err, arg);
    fprintf(stderr, "\n");
    print_usage();
    exit(-1);
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    int pid = 0;
    struct pollfd pfd;
    struct input_event event;

    // check number of arguments
    if (argc < 2 || argc > 5) {
        print_error_usage_exit("wrong number of arguments\n");
    }

    // initial file open flag
    int open_flag = O_RDONLY;
    bool is_exclusive = false;
    int index = 2;
    while (index < argc) {
        if (strcmp(argv[index], "-exclusive") == 0) {
            open_flag = O_RDWR;
            is_exclusive = true;
            index++;
        }
        else if (strcmp(argv[index], "-pid") == 0) {
            if (index + 1 == argc) print_error_usage_exit("-pid: missing PID\n"); 
            pid = atoi(argv[index+1]);
            if (pid == 0) print_error_usage_exit("-pid: invalid PID\n");
            index += 2;
        }
        else {
            print_error_arg_usage_exit("invalid argument %s\n", argv[2]);
        }
    }

    // open input device
    fd = open(argv[1], open_flag );
    if(fd <= 0) print_error_arg_usage_exit("could not open %s\n", argv[1]);

    // "grab" the device so other process cannot access it
    if (is_exclusive) {
        sleep(1);
        ioctl(fd, EVIOCGRAB, 1);
    }

    pfd.fd = fd;
    pfd.events = POLLIN;

    while(1) {

        // check if specified process exists, if not break the loop
        if (pid !=0 && kill(pid, 0) != 0) break;

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

    close(fd);

    return 0;
}