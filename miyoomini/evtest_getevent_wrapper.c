#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    const char *EVDEV = "/dev/input/event0";
    char line[512];
    

    // Open a pipe to evtest
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "evtest %s", EVDEV);
    FILE *pipe = popen(cmd, "r");
    if (!pipe) {
        fprintf(stderr, "Failed to run evtest\n");
        return 1;
    }

    setvbuf(stdout, NULL, _IOLBF, 0);  // line-buffered
    while (fgets(line, sizeof(line), pipe)) {
        if (strstr(line, "code 1 (KEY_ESC), value 1")) {
            printf("key B_MENU 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 1 (KEY_ESC), value 0")) {
            printf("key B_MENU 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 28 (KEY_ENTER), value 1")) {
            printf("key B_START 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 28 (KEY_ENTER), value 0")) {
            printf("key B_START 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 97 (KEY_RIGHTCTRL), value 1")) {
            printf("key B_SEL 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 97 (KEY_RIGHTCTRL), value 0")) {
            printf("key B_SEL 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 18 (KEY_E), value 1")) {
            printf("key B_L1 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 18 (KEY_E), value 0")) {
            printf("key B_L1 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 15 (KEY_TAB), value 1")) {
            printf("key B_L2 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 15 (KEY_TAB), value 0")) {
            printf("key B_L2 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 20 (KEY_T), value 1")) {
            printf("key B_R1 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 20 (KEY_T), value 0")) {
            printf("key B_R1 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 14 (KEY_BACKSPACE), value 1")) {
            printf("key B_R2 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 14 (KEY_BACKSPACE), value 0")) {
            printf("key B_R2 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 115 (KEY_VOLUMEUP), value 1")) {
            printf("key B_VOLUP 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 115 (KEY_VOLUMEUP), value 0")) {
            printf("key B_VOLUP 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 114 (KEY_VOLUMEDOWN), value 1")) {
            printf("key B_VOLDOWN 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 114 (KEY_VOLUMEDOWN), value 0")) {
            printf("key B_VOLDOWN 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 29 (KEY_LEFTCTRL), value 1")) {
            printf("key B_B 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 29 (KEY_LEFTCTRL), value 0")) {
            printf("key B_B 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 57 (KEY_SPACE), value 1")) {
            printf("key B_A 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 57 (KEY_SPACE), value 0")) {
            printf("key B_A 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 42 (KEY_LEFTSHIFT), value 1")) {
            printf("key B_X 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 42 (KEY_LEFTSHIFT), value 0")) {
            printf("key B_X 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 56 (KEY_LEFTALT), value 1")) {
            printf("key B_Y 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 56 (KEY_LEFTALT), value 0")) {
            printf("key B_Y 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 103 (KEY_UP), value 1")) {
            printf("key B_UP 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 103 (KEY_UP), value 0")) {
            printf("key B_UP 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 105 (KEY_LEFT), value 1")) {
            printf("key B_LEFT 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 105 (KEY_LEFT), value 0")) {
            printf("key B_LEFT 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 106 (KEY_RIGHT), value 1")) {
            printf("key B_RIGHT 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 106 (KEY_RIGHT), value 0")) {
            printf("key B_RIGHT 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 108 (KEY_DOWN), value 1")) {
            printf("key B_DOWN 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 108 (KEY_DOWN), value 0")) {
            printf("key B_DOWN 0\n");
            fflush(stdout);
        } else if (strstr(line, "code 116 (KEY_POWER), value 1")) {
            printf("key B_POWER 1\n");
            fflush(stdout);
        } else if (strstr(line, "code 116 (KEY_POWER), value 0")) {
            printf("key B_POWER 0\n");
            fflush(stdout);
        }
    }

    pclose(pipe);
    return 0;
}
