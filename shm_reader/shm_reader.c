#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <shmid> <output_file>\n", argv[0]);
        return 1;
    }

    int shmid = atoi(argv[1]);
    char *output = argv[2];

    char *data = (char *)shmat(shmid, NULL, SHM_RDONLY);
    if (data == (char *)-1) {
        perror("shmat");
        return 1;
    }

    struct shmid_ds buf;
    shmctl(shmid, IPC_STAT, &buf);
    size_t size = buf.shm_segsz;

    FILE *fp = fopen(output, "wb");
    if (!fp) {
        perror("fopen");
        shmdt(data);
        return 1;
    }

    fwrite(data, 1, size, fp);
    fclose(fp);
    shmdt(data);

    return 0;
}