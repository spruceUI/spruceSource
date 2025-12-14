#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s COMMAND [PORT]\n", argv[0]);
        return 1;
    }

    const char *cmd = argv[1];
    int port = (argc >= 3) ? atoi(argv[2]) : 55355;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sendto(sock, cmd, strlen(cmd), 0,
           (struct sockaddr *)&addr, sizeof(addr));

    close(sock);
    return 0;
}
