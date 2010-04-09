/*Written By: Scott Hollwedel
*/
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int initialize_compass()
{
	struct termios compass_term;
	
	// Initialize serial to compass
	int tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
   	tcgetattr(tty, &compass_term);
	//cfmakeraw(&compass_term);
  	compass_term.c_cflag = B19200;
   	//compass_term.c_cflag = CS8 | CREAD | CLOCAL;
   	//compass_term.c_iflag = IGNPAR | IGNBRK;
	tcsetattr(tty,TCSANOW,&compass_term);
	usleep(50000);
	
	/*//Activate True Heading Output
	char mode[] = " *4 ";
	mode[0] = 27;
	write(tty, &mode, sizeof(mode));
	usleep(50000);*/
	
	//Change local declination value(not sure what this is yet)
	char dec[] = " Q0 ";
	dec[0] = 27;
	write(tty, &dec, sizeof(dec));
	usleep(50000);
	return tty;	
}


double get_heading(int tty)
{
	char buffer[7];
	double heading = 0;
	tcflush(tty, TCIFLUSH); //Flush input to get newest data
	sleep(1); //Wait to fill up buffer to get new data out, this is dependent on frequency of output sentences
	read(tty, buffer, 7);
	if (buffer[6] == 'P');
		heading = (buffer[2] * 10) + buffer[3] + (buffer[5]*.1);
	else;
		heading = (buffer[2] * 100) + (buffer[3] * 10) + buffer[4] + (buffer[6]*.1);
	return heading;
}

int main()
{
	int tty = initialize_compass();
	while(1)
	{
		printf("Heading: %f\n", get_heading(tty));
	}
	close(tty);	
	return 0;
}
