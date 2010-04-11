#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static int tty;

int initialize_compass()
{
  struct termios compass_term;
  fprintf(stderr,"init:1.\n");

  tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
  fprintf(stderr,"init:1. tty: %d\n",tty);
  tcgetattr(tty, &compass_term);
  fprintf(stderr,"init:2.\n");
  cfmakeraw(&compass_term);
  fprintf(stderr,"init:3.\n");
  cfsetspeed(&compass_term, B19200);
  fprintf(stderr,"init:4.\n");
  compass_term.c_cflag = CS8 | CLOCAL | CREAD;
  fprintf(stderr,"init:5.\n");
  compass_term.c_iflag = IGNPAR | IGNBRK;
  fprintf(stderr,"init:6.\n");
  compass_term.c_cc[VTIME] = 10;
  fprintf(stderr,"init:7.\n");
  compass_term.c_cc[VMIN] = 0;
  fprintf(stderr,"init:8.\n");
  tcsetattr(tty,TCSANOW,&compass_term);
  fprintf(stderr,"init:9.\n");
  usleep(50000);
  fprintf(stderr,"init:10.\n");

  return tty;
}

//Read compass, return double of heading
double compass_get_heading()
{
    unsigned char buf[64]; //Fill up with each line
    int found = 0;
    int count = 0;
    double value = 0;
    
    //Disregard data until $C is found
    while( found != 1)
    {
        fprintf(stderr,"init:Getting: notfound.\n");
        read(tty, &buf[0], 1);
        fprintf(stderr,"init:Getting: character0: %c.\n", buf[0]);
        if(buf[0] == '$')
        {
            read(tty, &buf[0], 1);
            fprintf(stderr,"init:Getting: character1: %c.\n", buf[0]);
            if(buf[0] == 'C')
            {
                read(tty,&buf[0],5);
                fprintf(stderr,"init:Getting: character2: %c.\n", buf[0]);
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
    fprintf(stderr,"init:Getting: Done, return: %f.\n", value);
    return (value < 360.0 && value >= 0) ? value : -1;
}
