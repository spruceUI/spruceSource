// Author: oscar (https://github.com/oscarkcau)
//
// Set brightness and volume levels to shared memory for MainUI updating its interface 
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int shm_id;
    unsigned char *data;
    int n = 84;

    shm_id = shmget(0x594d4d4b, n, IPC_CREAT|0666);

    data = shmat(shm_id, 0, 0);

    if (argc == 1) {
        for (int i = 0; i < n; i++)
        {
            if (i > 0) printf(":");
            printf("%02X", data[i]);
        }
        printf("\n");
    } else {
        data[24] = atoi(argv[1]);
        data[28] = atoi(argv[2]);
    }

    shmdt(data);

    exit(0);
}