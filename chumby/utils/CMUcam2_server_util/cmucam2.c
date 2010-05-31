#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "cmucam2.h"

static int tty;

int cmucam2_init()
{
    struct termios cam_term;

#if 0
    tty = open("/dev/cam", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(tty < 0)
        return -1;

    tcgetattr(tty, &cam_term);
    cfmakeraw(&cam_term);
    cam_term.c_cflag = B115200;
    tcsetattr(tty,TCSAFLUSH,&cam_term);
#endif
    tty = open("/dev/cam", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(tty < 0)
        return -1;

    //system("stty -F /dev/cam 115200");
    tcgetattr(tty, &cam_term);
    cfmakeraw(&cam_term);
    // Set input rx speed
    if(cfsetispeed(&cam_term, B115200) < 0)
    {
        fprintf(stderr, "invalid baud rate");
        return EXIT_FAILURE;
    }
    // Set tx speed
    if(cfsetospeed(&cam_term, B115200) < 0)
    {
        fprintf(stderr, "invalid baud rate");
        return EXIT_FAILURE;
    }
    tcsetattr(tty,TCSAFLUSH,&cam_term);
    tcflush(tty, TCIOFLUSH);
    usleep(50000);
    return 0;
}

int cmucam2_dump_raw(unsigned char * buf, int size) {
    int bytes_read=0,i=0, j=0, total_bytes=0, row=0, xsize=0, ysize=0, cols=0;
    char cmd[]= "SF\r";

    // have cmucam send a frame
    write(tty,cmd,strlen(cmd));
    //fprintf(stderr, "cmd: %s\n", cmd);

    // read in frame bytes and store in buf
    while (1) {
        // wait for bytes
        usleep(19000);
        bytes_read = read(tty, &buf[total_bytes], size);
        if(bytes_read <= 0)
            break;
        total_bytes += bytes_read;
        fprintf(stderr, "[%d]", bytes_read);
    }

    fprintf(stderr, "\n\n[row %d]", ++row);
    for(i=4, j=0; i<total_bytes; ++i) {
        //New Frame
        if (buf[i] == 1) {
            xsize = buf[++i];
            ysize = buf[++i];
            continue;
        // New Row
        } else if(buf[i] == 2) {
            fprintf(stderr, "\n\n[row %d]", ++row);
            continue;
        // End of Frame
        } else if (buf[i] == 3) {
            buf[j] = 0;
            break;
        }

        buf[j++] = buf[i];
        if(row == 239)
            fprintf(stderr, "(%d)",++cols);
        fprintf(stderr, "%02x ", buf[i]);

    }
    fprintf(stderr, "\ncols: %d xsize: %d ysize: %d total_bytes: %d\n",cols, xsize, ysize, total_bytes);
    return j;
}
