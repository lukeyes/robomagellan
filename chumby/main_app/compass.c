#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "compass.h"

// Must match compass error codes in compass.h
const char * compass_err_msgs[NUM_COMPASS_ERRORS] = {
    "NO_ERROR",
    "COMPASS_OPEN_TTY_FAIL",
    "COMPASS_TTY_READ_FAIL",
    "COMPASS_DATA_CORRUPT",
    "COMPASS_DATA_TIMEOUT"
};

static int tty;

int compass_init()
{
  struct termios compass_term;

  tty = open("/dev/compass", O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (!tty)
    return COMPASS_OPEN_TTY_FAIL;
  tcgetattr(tty, &compass_term);
  cfmakeraw(&compass_term);
  cfsetspeed(&compass_term, B19200);
  compass_term.c_cflag = CS8 | CLOCAL | CREAD;
  compass_term.c_iflag = IGNPAR | IGNBRK;
  compass_term.c_cc[VTIME] = 10;
  compass_term.c_cc[VMIN] = 0;
  tcsetattr(tty,TCSANOW,&compass_term);
  usleep(50000);

  return COMPASS_NO_ERROR;
}

//Read compass, return double of heading
double compass_get_heading()
{
    unsigned char buf[64]; //Fill up with each line
    int found = 0;
    int count = 0;
    double value = 0;
    int retries = 0;
    
        tcflush(tty,TCIFLUSH);
        usleep(100000);
    
    //Disregard data until $C is found
    while( found != 1)
    {
        if(retries++ > 50)
          return COMPASS_DATA_TIMEOUT;

        if(read(tty, buf, 1) < 0)
            usleep(500000);

        if(buf[0] == '$')
        {
            usleep(10000);
            read(tty, buf, 1);
            if(buf[0] == 'C')
            {
                read(tty, buf,5);
                for (count = 0; count < 5 && buf[count] != '.'; count++)
                {
                    value *= 10;
                    value += buf[count] - '0';
                }
                value += ((buf[count+1] - '0') * 0.1);
                found = 1;
            }
        }
    }
    return (value < 360.0 && value >= 0) ? value : COMPASS_DATA_CORRUPT;
}
