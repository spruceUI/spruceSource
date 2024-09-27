#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN        (16 * (EVENT_SIZE + 16))

int main(int argc , char* argv[]) {
	if (argc<2) {
        puts("Simplify inotifywait");
		puts("Usage: inotify file_path");
        puts("only return after file modification");
		return 0;
	}
	
	char path[512];
	strncpy(path,argv[1],512);
	if (access(path, F_OK)!=0) return 0;

    int fd;
    fd = inotify_init();
    if (fd < 0)
        perror("inotify_init()");

    int wd;  
    wd = inotify_add_watch(fd, path, IN_MODIFY);
    if (wd < 0)
        perror("inotify_add_watch");
    char buf[BUF_LEN];
    int len;
    len = read(fd, buf, BUF_LEN);

    if (len > 0)
    {
        int i = 0;
        while (i < len)
        {
            struct inotify_event *event;
            event = (struct inotify_event *) &buf[i];

            printf("wd=%d mask=%x cookie=%u len=%u\n",
                event->wd, event->mask,
                event->cookie, event->len);

            if (event->mask & IN_MODIFY){
                printf("file modified %s", event->name);
                return 0;
            }
            if (event->mask & IN_IGNORED) {
                inotify_rm_watch(fd, wd);
                wd = inotify_add_watch(fd, path, IN_MODIFY);
            }

            if (event->len)
                printf("name=%s\n", event->name);

            i += EVENT_SIZE + event->len;
        }
        
    }



	return 0;
}
