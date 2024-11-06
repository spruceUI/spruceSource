#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "wrong number of arguments\n");
        fprintf(stderr, "usage: devlogger identity message\n");
        exit(0);
    }

    openlog (argv[1], LOG_CONS | LOG_PID, LOG_USER);

    syslog(LOG_DEBUG, argv[2]);

    closelog();
}