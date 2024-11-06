// Author: oscar (https://github.com/oscarkcau)
//
// CLI tool that reads input events from stdin and send events to specified input device.
//
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <poll.h>
#include <unistd.h>

void print_usage();

void print_usage() {
    fprintf(stdout, "usage: sendevent input_device\n");
    fprintf(stdout, "Read input event from stdin and send event to input_device.\n\n");
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    char line[1024];
    unsigned short type, code;
    int value;
    struct input_event event;

    // check number of arguments
    if (argc != 2) {
        fprintf(stderr, "wrong number of arguments\n");
        print_usage();
        return -1;
    }

    // open input device
    fd = open(argv[1], O_RDWR);
    if(fd <= 0) {
        fprintf(stderr, "could not open %s\n", argv[1]);
        print_usage();
        return -1;
    }

    while (1) {

        // read input event code from stdin
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%hu %hu %d", &type, &code, &value);

        // exit when "0 0 0" is read
        if (type == 0 && code == 0 && value == 0) {
            fsync(fd);
            close(fd);
            return 0;
        }

        // send event to input device
        event.type = type;
        event.code = code;
        event.value = value;
        ret = write( fd, &event, sizeof(struct input_event) );
        if (ret == -1) {
            fprintf(stderr, "could not write event %hu %hu %d\n", type, code, value);
        }
        
        // send event separator to input device
        event.type = 0;
        event.code = 0;
        event.value = 0;
        ret = write( fd, &event, sizeof(struct input_event) );
        if (ret == -1) {
            fprintf(stderr, "could not write event 0 0 0\n");
        }
    }

    return 0;
}