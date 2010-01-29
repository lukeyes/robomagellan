/*	A module that has functions that will read from the
 *	specified sonar sensor, the distance from that that
 *	sensor to the nearest object in inches.
 *
 *	This module also contains a main driver for testing.
 *	For debug mode use -d flag.
 *
 *	Note: Tested on 6/13/2009. Ran continuously for 20+ minutes
 *		  without faulting. This should be sufficient for completion
 *		  of the task.
 *
 *  Author:	Michael Ortiz (mtortiz.mail@gmail.com)
 *  Modified:	Scott Hollwedel (s.hollwedel@gmail.com)(1/22/10)
 */

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define I2C_CMD   0x55
#define SONAR_LAST_ADDR 0xE0

int debug = 0; //Set to 1 if want debug messages displayed
static int to = 0; //Timeout flag

void timeout()
{
	to = 1;
}

int initialize_i2c()
{
	struct termios i2c_term;
	int tty;
	//Took out O_NDELAY
	tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);

   	/*Set serial port settings*/
	tcgetattr(tty,&i2c_term);
	cfsetispeed(&i2c_term, B19200);
	cfsetospeed(&i2c_term, B19200);
	i2c_term.c_cflag |= (CLOCAL | CREAD);
	i2c_term.c_cflag |= CSTOPB; //2 Stop bits
	i2c_term.c_cflag &= ~PARENB;//No parity
	i2c_term.c_cflag &= ~CSIZE;
   	i2c_term.c_cflag |= CS8;//8 bits  			
 
	tcflush(tty, TCIFLUSH) ;				
        tcsetattr(tty,TCSANOW,&i2c_term) ;			

	
	return tty;	
}

int initialize_sonar(int tty)
{
	char buf[5];
	int address = 0xE0; //Address of 1st sensor

	// Set new range(6m)
	while ( address <= SONAR_LAST_ADDR )
	{
		buf[0] = I2C_CMD; 
		buf[1] = address; //Device add + w/r bit
		buf[2] = 0x02; //Register
		buf[3] = 0x01; //# of bytes
		buf[4] = 140; //Data - 140 corresponds to 6m
		write(tty, &buf, sizeof(buf));
		printf("Wrote new range to: %x\n",address);
		address= address + 2;
		sleep(1);
		tcflush(tty,TCIOFLUSH);
	}

	address = 0xE0;
	// Set new gain corresponding to range
	while ( address <= SONAR_LAST_ADDR )
	{
		buf[0] = I2C_CMD; 
		buf[1] = address; //Device add + w/r bit
		buf[2] = 0x01; //Register
		buf[3] = 0x01; //# of bytes
		buf[4] = ; //Data - 31 is default(play with to decrease time)
		write(tty, &buf, sizeof(buf));
		printf("Wrote new gain to: %x\n",address);
		address = address + 2;
		sleep(1);
		tcflush(tty,TCIOFLUSH);
	}

	return 0;
}

int take_range( int tty )
{
	char buf[5];
	int address = 0xE0; //Address of 1st sensor

	// Take range
	while ( address <= SONAR_LAST_ADDR )
	{
		buf[0] = I2C_CMD; 
		buf[1] = address; //Device add + w/r bit
		buf[2] = 0x00; //Register
		buf[3] = 0x01; //# of bytes
		buf[4] = 80; //Data - 80 = inches, 81 = cm, 82 = uc
		write(tty, &buf, sizeof(buf));
		address = address + 2;
	}

	return 0;
}

int get_range(int tty, unsigned int * output)
{
	char buf[4];
	unsigned char outbuf[1];
	int address = 0xE0; //Address of 1st sensor
	int count = 0;
	int reg = 2;

	// Get range
	while ( address <= SONAR_LAST_ADDR )
	{
		tcflush(tty,TCIFLUSH);
		outbuf[0] = 0;
		outbuf[1] = 0;

		//Get high bit
		buf[0] = I2C_CMD; 
		buf[1] = address + 1; //Device add + w/r bit
		buf[2] = 0x02; //Register
		buf[3] = 0x01; //# of bytes
		//output[count] = outbuf[0] << 8;
		printf("Reading Registers...\n");
		while(reg < 36)
		{
			buf[2] = reg;
			if(write(tty, &buf, sizeof(buf)) < 0)
			{
				perror("Write failed");
			}
			printf("Reading Register\n");	
			if(read(tty,&outbuf,1) < 0)
			{
				perror("Read failed");
			}
			
			
			printf("Address: %x, Reg: %d, Value: %d\n",address, reg, outbuf[0]);
			reg++;
		}
		//output[count] = outbuf[0] + output[count];

		address = address + 2;
		count++;
	}
	return 0;
} 


//For Debug
int main()
{	
	int tty = initialize_i2c();
	unsigned int buf[2];
	int count = 0;

	initialize_sonar(tty);
	printf("Finished Initializing...\n");
	
	while( count < 1 )
	{
		printf("Taking Range...\n");
		take_range(tty);
		usleep(70000);
		printf("Getting Range..\n");
		get_range(tty, buf);
		//printf("Ouput: %d %d\n", buf[0], buf[1] );
		count++;
	}
	close(tty);
	return 0;

}


