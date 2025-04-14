#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h> /* to handle framebuffer ioctls */

#define	DEFAULT_FB	"/dev/fb0"

int main(int argc, char *argv[])
{
    int fd;
    struct fb_var_screeninfo fb_varinfo;

    if(-1 == (fd=open(DEFAULT_FB, O_RDWR)))
    {
        fprintf(stderr, "Error: Couldn't open %s.\n", DEFAULT_FB);
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_varinfo) != 0)
	    fprintf(stderr, "ioctl FBIOGET_VSCREENINFO");

    fb_varinfo.red.offset = 16;
    fb_varinfo.red.length = 8;
    fb_varinfo.green.offset = 8;
    fb_varinfo.green.length = 8;
    fb_varinfo.blue.offset = 0;
    fb_varinfo.blue.length = 8;
    fb_varinfo.transp.offset = 24;
    fb_varinfo.transp.length = 8;

    if (ioctl(fd, FBIOPUT_VSCREENINFO, &fb_varinfo) != 0)
	    fprintf(stderr, "ioctl FBIOPUT_VSCREENINFO");

    (void) close(fd);

    exit(0);
}