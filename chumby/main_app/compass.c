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
    "COMPASS_OPEN_FAIL",
    "COMPASS_READ_FAIL",
    "COMPASS_DATA_CORRUPT",
    "COMPASS_DATA_TIMEOUT"
};

static int tty=0;

int compass_init()
{
  struct termios compass_term;

    // If a reinit occurs
  if(tty)
    close(tty);

  tty = open("/dev/compass", O_RDWR | O_NOCTTY | O_NONBLOCK);
  system("stty -F /dev/compass 19200");
  if (tty < 0)
    return COMPASS_OPEN_FAIL;
  tcgetattr(tty, &compass_term);
  cfmakeraw(&compass_term);
  cfsetspeed(&compass_term, B19200);
  compass_term.c_cflag = CS8 | CLOCAL | CREAD;
  compass_term.c_iflag = IGNPAR | IGNBRK;
  compass_term.c_lflag = ICANON;
  compass_term.c_cc[VTIME] = 10;
  compass_term.c_cc[VMIN] = 0;
  tcflush(tty, TCIFLUSH);
  tcsetattr(tty,TCSANOW,&compass_term);
  //usleep(50000);

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
    
    //Disregard data until $C is found
    while( found != 1)
    {
		while (read(tty, buf, 64) <= 0)
		{
            usleep(5000);
            if(retries++ > 50)
                return COMPASS_DATA_TIMEOUT;
        }

        if(buf[0] == '$')
        {
            if(buf[1] == 'C')
            {
                for (count = 2; count < 7 && buf[count] != '.'; count++)
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

#if 0
main()
{
    double x = 0;
    compass_init();
    while (1)
    {
        x = compass_get_heading();
    	printf("value: %f\n",x);
    }
}
#endif
