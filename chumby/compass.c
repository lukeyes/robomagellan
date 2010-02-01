#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define COMPASS_ADDRESS 0x42


int intitialize_compass(int tty)
{
	
}

double get_heading(int tty)
{
	double heading = 0;

	//Read heading
	write_i2c(tty,COMPASS_ADDRESS+1,0x41,1,0);

	//Might read 1 byte ack bit b4 getting data
	read(tty, &heading, 2);
	return heading;
}

//For debug
main () 
{
	double heading;
	int tty = initialize_i2c();
	heading = get_heading(tty);
}
