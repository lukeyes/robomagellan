#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "cmucam2.h"

static int tty;

int cmucam2_init()
{
    struct termios cam_term;

    tty = open("/dev/cam", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(tty < 0)
        return -1;

    tcgetattr(tty, &cam_term);
    cfmakeraw(&cam_term);
    cam_term.c_cflag = B115200;
    tcsetattr(tty,TCSAFLUSH,&cam_term);
    usleep(50000);
    return 0;
}

int cmucam2_dump_raw(void * buf, int size) {
    int bytes_read;
    char resp[4096];
    char cmd1[] = "LM 0 1\r";
    char cmd2[] = "tc\r";

#if 0
    // Enable raw image data to be transfered after a TC command
    write(tty,"LM 0 1\r",sizeof(cmd1));
    bytes_read = read(tty, resp, sizeof(resp));
    fprintf(stderr, "len: %d resp: %s\n", bytes_read, resp);
#endif
    // Send TC command to get ram image data
    write(tty,cmd2,sizeof(cmd2));
    bytes_read = read(tty, resp, sizeof(resp));
    fprintf(stderr, "len: %d resp: %s\n", bytes_read, resp);
    return 0;
}
