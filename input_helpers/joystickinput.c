#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <stdbool.h>
#include <signal.h>

#define MIYOO_AXIS_MAX_COUNT 16
#define MIYOO_PLAYER_MAGIC 0xFF
#define MIYOO_PLAYER_MAGIC_END 0xFE
#define MIYOO_PAD_FRAME_LEN 6
#define KEYBOARD_MODE 1
#define ANALOG_MODE 2

struct MIYOO_PAD_FRAME
{
    uint8_t magic;
    uint8_t unused0;
    uint8_t unused1;
    uint8_t axis0;
    uint8_t axis1;
    uint8_t magicEnd;
};
static int s_fd = -1;
static int g_lastX = 0;
static int g_lastY = 0;
static int MIYOO_ADC_MAX_X = 200;
static int MIYOO_ADC_ZERO_X = 137;
static int MIYOO_ADC_MIN_X = 76;
static int MIYOO_ADC_MAX_Y = 200;
static int MIYOO_ADC_ZERO_Y = 135;
static int MIYOO_ADC_MIN_Y = 72;
static int MIYOO_ADC_DEAD_ZONE = 10;
static int MIYOO_AXIS_INT8_DRIFT = 5;
static struct MIYOO_PAD_FRAME s_frame = {0};
static int32_t s_miyoo_axis[MIYOO_AXIS_MAX_COUNT] = {0};
static int32_t s_miyoo_axis_last[MIYOO_AXIS_MAX_COUNT] = {0};
static bool hasAnalogOutput = false;
static bool hasKeyboardOutput = false;
static int outputMode = ANALOG_MODE;
static int fd_key, fd_axis;

int uart_open(const char *port)
{
    int fd = -1;

    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd)
    {
        printf("Failed to open uart\n");
        return -1;
    }

    if (fcntl(fd, F_SETFL, 0) < 0)
    {
        printf("Failed to call fcntl\n");
        return -1;
    }
    return fd;
}
static void uart_close(int fd)
{
    close(fd);
}
static int uart_init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
    int i = 0;
    int speed_arr[] = {B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int name_arr[] = {115200, 19200, 9600, 4800, 2400, 1200, 300};
    struct termios options = {0};

    if (tcgetattr(fd, &options) != 0)
    {
        printf("Failed to get uart attributes\n");
        return -1;
    }

    for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);
            cfsetospeed(&options, speed_arr[i]);
        }
    }

    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    switch (flow_ctrl)
    {
    case 0:
        options.c_cflag &= ~CRTSCTS;
        break;
    case 1:
        options.c_cflag |= CRTSCTS;
        break;
    case 2:
        options.c_cflag |= IXON | IXOFF | IXANY;
        break;
    }

    options.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 5:
        options.c_cflag |= CS5;
        break;
    case 6:
        options.c_cflag |= CS6;
        break;
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        return -1;
    }

    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 's':
    case 'S':
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        return -1;
    }

    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        return -1;
    }

    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 1;

    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &options) != 0)
    {
        return -1;
    }
    return 0;
}

static int uart_read(int fd, char *rcv_buf, int data_len)
{
    int f_sel;
    fd_set f_read;
    struct timeval time = {0};

    FD_ZERO(&f_read);
    FD_SET(fd, &f_read);

    time.tv_sec = 10;
    time.tv_usec = 0;
    f_sel = select(fd + 1, &f_read, NULL, NULL, &time);
    if (f_sel)
    {
        return read(fd, rcv_buf, data_len);
    }
    return 0;
}

static int filterDeadzone(int newAxis, int oldAxis)
{
    if (abs(newAxis - oldAxis) < 3)
    {
        return 1;
    }
    return 0;
}
static int limitValue8(int value)
{
    if (value > 127)
    {
        value = 127;
    }
    else if (value < -128)
    {
        value = -128;
    }
    return value;
}
static void check_axis_event(void)
{
    int i = 0;
    int value, old_value;
    int ret;
    unsigned short pos_code, neg_code;
    struct input_event event;
    struct input_event dummy_event = { {0,0}, EV_SYN, 0, 0};

    for (i = 0; i < 2; i++)
    {
        if (s_miyoo_axis[i] != s_miyoo_axis_last[i])
        {
            //if (!filterDeadzone(s_miyoo_axis[i], s_miyoo_axis_last[i]))
            {
                ret = 0;

                if (outputMode == ANALOG_MODE) {
                    event.type = EV_ABS;
                    event.code = i;
                    event.value = s_miyoo_axis[i];
                    if (hasAnalogOutput) {
                        ret = write( fd_axis, &event, sizeof(struct input_event) );
                        write( fd_axis, &dummy_event, sizeof(struct input_event) );
                    } else {
                        fprintf(stdout, "%d %d %d\n", EV_ABS, i, s_miyoo_axis[i]);
                        fflush(stdout);
                    }
                } else if (outputMode == KEYBOARD_MODE) {
                    value = s_miyoo_axis[i];
                    old_value = s_miyoo_axis_last[i];
                    if (i == 0) { pos_code = KEY_RIGHT; neg_code = KEY_LEFT; }
                    else if (i == 1) { pos_code = KEY_DOWN; neg_code = KEY_UP; }
                        
                    event.type = 0;
                    if (value >= 64 && old_value < 64) {
                        event.type = EV_KEY;
                        event.code = pos_code;
                        event.value = 1;
                    } else if (value < 64 && old_value >= 64) {
                        event.type = EV_KEY;
                        event.code = pos_code;
                        event.value = 0;
                    } else if (value <= -64 && old_value > -64) {
                        event.type = EV_KEY;
                        event.code = neg_code;
                        event.value = 1;
                    } else if (value > -64 && old_value <= -64) {
                        event.type = EV_KEY;
                        event.code = neg_code;
                        event.value = 0;
                    }
                    
                    if (event.type == EV_KEY) {
                        if (hasKeyboardOutput) {
                            ret = write( fd_key, &event, sizeof(struct input_event) );
                            write( fd_key, &dummy_event, sizeof(struct input_event) );
                        } else {
                            fprintf(stdout, "%hu %hu %d\n", event.type, event.code, event.value);
                            fflush(stdout);
                        }
                    }
                }

                if (ret == -1) {
                    fprintf(stderr, "could not write event %hu %hu %d\n", event.type, event.code, event.value);
                }
            }
        }
        s_miyoo_axis_last[i] = s_miyoo_axis[i];
    }
    fflush(stdout);
}
static int miyoo_frame_to_axis_x(uint8_t rawX)
{
    int value = 0;

    if (rawX > MIYOO_ADC_ZERO_X)
    {
        value = (rawX - MIYOO_ADC_ZERO_X) * 126 / (MIYOO_ADC_MAX_X - MIYOO_ADC_ZERO_X);
    }

    if (rawX < MIYOO_ADC_ZERO_X)
    {
        value = (rawX - MIYOO_ADC_ZERO_X) * 126 / (MIYOO_ADC_ZERO_X - MIYOO_ADC_MIN_X);
    }

    if (value > 0 && value < MIYOO_ADC_DEAD_ZONE)
    {
        return 0;
    }

    if (value < 0 && value > -(MIYOO_ADC_DEAD_ZONE))
    {
        return 0;
    }
    return value;
}
static int miyoo_frame_to_axis_y(uint8_t rawY)
{
    int value = 0;

    if (rawY > MIYOO_ADC_ZERO_Y)
    {
        value = (rawY - MIYOO_ADC_ZERO_Y) * 126 / (MIYOO_ADC_MAX_Y - MIYOO_ADC_ZERO_Y);
    }

    if (rawY < MIYOO_ADC_ZERO_Y)
    {
        value = (rawY - MIYOO_ADC_ZERO_Y) * 126 / (MIYOO_ADC_ZERO_Y - MIYOO_ADC_MIN_Y);
    }

    if (value > 0 && value < MIYOO_ADC_DEAD_ZONE)
    {
        return 0;
    }

    if (value < 0 && value > -(MIYOO_ADC_DEAD_ZONE))
    {
        return 0;
    }
    return value;
}
static int parser_miyoo_input(const char *cmd, int len)
{
    int i = 0;
    int p = 0;

    if ((!cmd) || (len < MIYOO_PAD_FRAME_LEN))
    {
        return -1;
    }

    for (i = 0; i < len - MIYOO_PAD_FRAME_LEN + 1; i += MIYOO_PAD_FRAME_LEN)
    {
        for (p = 0; p < MIYOO_PAD_FRAME_LEN - 1; p++)
        {
            if ((cmd[i] == MIYOO_PLAYER_MAGIC) && (cmd[i + MIYOO_PAD_FRAME_LEN - 1] == MIYOO_PLAYER_MAGIC_END))
            {
                memcpy(&s_frame, cmd + i, sizeof(s_frame));
                break;
            }
            else
            {
                i++;
            }
        }
    }
    s_miyoo_axis[ABS_X] = miyoo_frame_to_axis_x(s_frame.axis0);
    s_miyoo_axis[ABS_Y] = miyoo_frame_to_axis_y(s_frame.axis1);
    check_axis_event();
    return 0;
}
static int miyoo_init_serial_input(const char * port)
{
    int err = 0;

    memset(&s_frame, 0, sizeof(s_frame));
    memset(s_miyoo_axis, 0, sizeof(s_miyoo_axis));
    memset(s_miyoo_axis_last, 0, sizeof(s_miyoo_axis_last));
    s_fd = uart_open(port);
    err = uart_init(s_fd, 9600, 0, 8, 1, 'N');
    if (s_fd <= 0 || err != 0)
    {
        return -1;
    }
    return 0;
}
static void miyoo_close_serial_input(void)
{
}

void print_usage() {
    fprintf(stderr, "usage: joystickinput serial_port calibration_file [-axis input_device] [-key input_device]\n");
    fprintf(stdout, "Read joystick input from serial port and convert to keyboard input or analog axis input.\n\n");
    fprintf(stdout, "-axis\tspecify input device that receives analog input.\n");
    fprintf(stdout, "-key\tspecify input device that receives keyboard input.\n");
}

static int read_joystick_config(const char * filename)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret;
    char name[1024];
    int value;
    
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1)
    {
        ret = sscanf(line, "%[^=]=%d", name, &value);
        if (ret != 2) continue;

        if (strcmp(name, "x_min") == 0) MIYOO_ADC_MIN_X = value;
        else if (strcmp(name, "x_max") == 0) MIYOO_ADC_MAX_X = value;
        else if (strcmp(name, "x_zero") == 0) MIYOO_ADC_ZERO_X = value;
        else if (strcmp(name, "y_min") == 0) MIYOO_ADC_MIN_Y = value;
        else if (strcmp(name, "y_max") == 0) MIYOO_ADC_MAX_Y = value;
        else if (strcmp(name, "y_zero") == 0) MIYOO_ADC_ZERO_Y = value;
    }

    fclose(fp);
    if (line)
        free(line);

    return 0;
}

static void sigusr(int signum)
{
    if(signum == SIGUSR1)
        outputMode = ANALOG_MODE;
    else if(signum == SIGUSR2)
        outputMode = KEYBOARD_MODE;
}

int main(int argc, char **argv)
{
    int len = 0;
    int ret;
    char rcv_buf[255] = {0};

    if(signal(SIGUSR1, sigusr) == SIG_ERR) {
        fprintf(stderr, "error catching SIGUSR1\n");
        return -1;
    }

    if(signal(SIGUSR2, sigusr) == SIG_ERR) {
        fprintf(stderr, "error catching SIGUSR2\n");
        return -1;
    }

    if (argc < 3 || argc > 7) {
        fprintf(stderr, "wrong number of arguments\n");
        print_usage();
        return -1;
    }

    ret = miyoo_init_serial_input(argv[1]);
    if (ret != 0) {
        fprintf(stderr, "could not open %s\n", argv[1]);
        print_usage();
        return -1;
    }

    ret = read_joystick_config(argv[2]);
    if (ret != 0) {
        fprintf(stderr, "could not open %s\n", argv[2]);
        print_usage();
        return -1;
    }

    int index = 3;
    while (index < argc - 1) {
        if (strcmp(argv[index], "-key") == 0) {
            hasKeyboardOutput = true;
            fd_key = open(argv[index + 1], O_RDWR);
            if(fd_key <= 0) {
                fprintf(stderr, "could not open %s\n", argv[index + 1]);
                print_usage();
                return -1;
            }
            index += 2;
        }
        else if (strcmp(argv[index], "-axis") == 0) {
            hasAnalogOutput = true;
            fd_axis = open(argv[index + 1], O_RDWR);
            if(fd_axis <= 0) {
                fprintf(stderr, "could not open %s\n", argv[index + 1]);
                print_usage();
                return -1;
            }
            index += 2;
        }
        else break;
    }
    if (index < argc) {
        fprintf(stderr, "invalid argument %s\n", argv[index]);
        print_usage();
        return -1;            
    }

    while (1)
    {
        len = uart_read(s_fd, rcv_buf, 99);
        if (len > 0)
        {
            rcv_buf[len] = '\0';
            parser_miyoo_input(rcv_buf, len);
        }
        usleep(10);
    }
    uart_close(s_fd);
    s_fd = -1;

    miyoo_close_serial_input();
    return 0;
}