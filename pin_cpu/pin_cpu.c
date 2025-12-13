#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void print_help() {
    printf("Usage:\n");
    printf("  pin_cpu <cpulist> -l|--launch <command> [args]\n");
    printf("  pin_cpu <cpulist> -p|--pid <PID>\n");
    printf("  pin_cpu <cpulist> -n|--name <process_name>\n");
    printf("Optional:\n");
    printf("  -d | --daemon   Keep enforcing affinity (new threads)\n\n");
    printf("cpulist example: 0,1,3 or 2 or 0-3\n");
}

// Parse CPU list like: "0,2,4-6"
int parse_cpu_list(const char *str, cpu_set_t *set) {
    CPU_ZERO(set);
    const char *p = str;

    while (*p) {
        int start, end;

        if (isdigit(*p)) {
            start = strtol(p, (char**)&p, 10);
            end = start;

            if (*p == '-') {
                p++;
                end = strtol(p, (char**)&p, 10);
            }

            for (int c = start; c <= end; c++)
                CPU_SET(c, set);

            if (*p == ',')
                p++;
        } else {
            return -1;
        }
    }
    return 0;
}

// Apply affinity to all threads of a process
int set_affinity_all_threads(pid_t pid, cpu_set_t *mask) {
    char taskdir[64];
    snprintf(taskdir, sizeof(taskdir), "/proc/%d/task", pid);

    DIR *d = opendir(taskdir);
    if (!d) return -1;

    struct dirent *e;
    int count = 0;

    while ((e = readdir(d))) {
        if (!isdigit(e->d_name[0]))
            continue;

        pid_t tid = atoi(e->d_name);
        if (sched_setaffinity(tid, sizeof(cpu_set_t), mask) == 0)
            count++;
    }

    closedir(d);
    return count;
}

// Read /proc/<pid>/comm to check name
int pid_matches_name(pid_t pid, const char *target) {
    char path[64], buf[256];
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);

    FILE *f = fopen(path, "r");
    if (!f) return 0;

    if (!fgets(buf, sizeof(buf), f)) {
        fclose(f);
        return 0;
    }
    fclose(f);

    buf[strcspn(buf, "\n")] = 0;
    return strcmp(buf, target) == 0;
}

int apply_by_name(const char *name, cpu_set_t *mask) {
    DIR *d = opendir("/proc");
    if (!d) return -1;

    struct dirent *e;
    int count = 0;

    while ((e = readdir(d))) {
        if (!isdigit(e->d_name[0]))
            continue;

        pid_t pid = atoi(e->d_name);
        if (pid_matches_name(pid, name)) {
            if (set_affinity_all_threads(pid, mask) > 0) {
                printf("Pinned PID %d (%s)\n", pid, name);
                count++;
            }
        }
    }

    closedir(d);
    return count;
}

// --- Daemon mode: continually reapply affinity ---
void daemon_enforce_pid(pid_t pid, cpu_set_t *mask) {
    while (1) {
        if (kill(pid, 0) != 0)
            exit(0);

        set_affinity_all_threads(pid, mask);
        usleep(200000);
    }
}

void daemon_enforce_name(const char *name, cpu_set_t *mask) {
    while (1) {
        int found = apply_by_name(name, mask);
        if (found <= 0)
            exit(0);

        usleep(200000);
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        print_help();
        return 1;
    }

    cpu_set_t mask;

    if (parse_cpu_list(argv[1], &mask) != 0) {
        fprintf(stderr, "Invalid CPU list: %s\n", argv[1]);
        return 1;
    }

    int daemon_flag = 0;
    const char *mode = NULL;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--daemon") == 0)
            daemon_flag = 1;
        else if (!mode)
            mode = argv[i];
    }

    if (!mode) {
        print_help();
        return 1;
    }

    // --- Launch mode ---
    if ((strcmp(mode, "-l") == 0 || strcmp(mode, "--launch") == 0)) {
        int cmd_index = -1;
        for (int i = 3; i < argc; i++)
            if (argv[i][0] != '-' || strcmp(argv[i], "-d") == 0)
                { cmd_index = i; break; }

        if (cmd_index == -1) {
            print_help();
            return 1;
        }

        pid_t child = fork();
        if (child == 0) {
            set_affinity_all_threads(0, &mask);
            execvp(argv[cmd_index], &argv[cmd_index]);
            perror("execvp");
            exit(1);
        }

        if (!daemon_flag) {
            int status;
            waitpid(child, &status, 0);
            return status;
        }

        if (fork() == 0)
            daemon_enforce_pid(child, &mask);

        return 0;

    // --- PID mode ---
    } else if ((strcmp(mode, "-p") == 0 || strcmp(mode, "--pid") == 0)) {

        pid_t pid = atoi(argv[3]);
        if (set_affinity_all_threads(pid, &mask) <= 0) {
            perror("sched_setaffinity");
            return 1;
        }

        printf("Pinned PID %d successfully.\n", pid);

        if (daemon_flag && fork() == 0)
            daemon_enforce_pid(pid, &mask);

        return 0;

    // --- Name mode ---
    } else if ((strcmp(mode, "-n") == 0 || strcmp(mode, "--name") == 0)) {

        const char *name = argv[3];
        int count = apply_by_name(name, &mask);

        if (count <= 0) {
            fprintf(stderr, "No processes matched name: %s\n", name);
            return 1;
        }

        if (daemon_flag && fork() == 0)
            daemon_enforce_name(name, &mask);

        return 0;
    }

    print_help();
    return 1;
}
