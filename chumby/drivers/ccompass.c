#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int initialize_ccompass()
{
  int tty;
  struct termios ccompass_term;

  tty = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY | O_NONBLOCK);
  tcgetattr(tty, &ccompass_term);
  cfmakeraw(&ccompass_term);
  cfsetspeed(&ccompass_term, B19200);
  ccompass_term.c_cflag = CS8 | CLOCAL | CREAD;
  ccompass_term.c_iflag = IGNPAR | IGNBRK;
  ccompass_term.c_cc[VTIME] = 10;
  ccompass_term.c_cc[VMIN] = 0;
  tcsetattr(tty,TCSAFLUSH,&ccompass_term);

  return tty;
}

//Read compass, return double of heading
double get_ccompass(int tty_ccompass)
{
   	unsigned char buf[64]; //Fill up with each line
	int found = 0;
	int count = 0;
	double value = 0;
	
	//Disregard data until $C is found
	while( found != 1)
	{
   		read(tty_ccompass, &buf[0], 1);
		if(buf[0] == '$')
		{
   			read(tty_ccompass, &buf[0], 1);
			if(buf[0] == 'C')
			{
				read(tty_ccompass,&buf[0],5);
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
	return (value < 360.0 && value >= 0) ? value : -1;
}

int main()
{
	int tty = initialize_ccompass();

	while (1)
		printf("%f\n",get_ccompass(tty));
}
