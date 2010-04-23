#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "switch.h"

// Must match switch error codes in switch.h
const char * switch_err_msgs[NUM_SWITCH_ERRORS] = {
    "SWITCH_NO_ERROR",
    "SWITCH_OPEN_TTY_FAIL"
};

static int tty;

int switch_init()
{
    tty = open("/dev/switch", O_RDONLY);
    if (!tty)
      return SWITCH_OPEN_TTY_FAIL;

    return SWITCH_NO_ERROR;
}

int switch_get_state()
{
    int x = -1;
    tcflush(tty, TCIFLUSH);
    read(tty, &x,1);
    return x & 1;
}
void switch_close() {
    close(tty);
}

#if 0
main()
{
    int x = 0;
    switch_init();
    while (1)
    {
        x = switch_get_state();
        printf("value: %d\n",x);
    }
}
#endif
