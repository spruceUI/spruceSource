// Reads AXP223 PMIC registers on the Mini Plus and Mini Flip and prints each one as 0xNN on its own line. No kernel
// driver owns the PMIC; it sits at 0x34 on /dev/i2c-1, where axp_test talks to it too. Read-only on purpose: there is
// no write path, so nothing calling this can change a charger or power setting.
//   axp_read 0x00 0xb9   -> 0xc5, 0xe4 (one per line)
// Exit status: 0 all read, 1 a read failed, 2 usage.
// Built by this folder's Dockerfile. The Mini has glibc 2.28: a gcc from a distro newer than Debian 10 links
// against 2.34, and the binary will not load.
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define AXP_BUS "/dev/i2c-1"
#define AXP_ADDR 0x34

static int parse_reg(const char *arg, unsigned char *reg)
{
    char *end;
    // Always hex, 0x optional: base 0 would read a leading zero as octal, so 010 would be register 0x08
    long value = strtol(arg, &end, 16);

    if (end == arg || *end != '\0' || value < 0 || value > 0xff)
        return -1;
    *reg = (unsigned char)value;
    return 0;
}

// One combined write-then-read transaction, so axp_test running at the same moment cannot land between the two
static int read_reg(int fd, unsigned char reg, unsigned char *value)
{
    struct i2c_msg msgs[2] = {
        { .addr = AXP_ADDR, .flags = 0, .len = 1, .buf = &reg },
        { .addr = AXP_ADDR, .flags = I2C_M_RD, .len = 1, .buf = value },
    };
    struct i2c_rdwr_ioctl_data data = { .msgs = msgs, .nmsgs = 2 };

    return ioctl(fd, I2C_RDWR, &data) == 2 ? 0 : -1;
}

int main(int argc, char **argv)
{
    unsigned char regs[256];
    int count = argc - 1;
    int status = 0;

    if (count < 1 || count > 256) {
        fprintf(stderr, "usage: axp_read REG [REG...]  (hex, 0x00-0xff)\n");
        return 2;
    }
    // Parse everything before touching the bus, so a bad argument never yields a partial listing
    for (int i = 0; i < count; i++) {
        if (parse_reg(argv[i + 1], &regs[i]) != 0) {
            fprintf(stderr, "axp_read: not a register: %s\n", argv[i + 1]);
            return 2;
        }
    }

    int fd = open(AXP_BUS, O_RDWR);
    if (fd < 0) {
        perror("axp_read: " AXP_BUS);
        return 1;
    }
    for (int i = 0; i < count; i++) {
        unsigned char value;

        if (read_reg(fd, regs[i], &value) != 0) {
            fprintf(stderr, "axp_read: register 0x%02x: read failed\n", regs[i]);
            status = 1;
            break;
        }
        printf("0x%02x\n", value);
    }
    close(fd);
    return status;
}
