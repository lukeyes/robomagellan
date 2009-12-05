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
 */

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define I2CD_CMD  0x53
#define I2C_CMD   0x55
#define CM01_CMD  0x5a

int debug = 0; //Set to 1 if want debug messages displayed


/*	This function takes the address of the USB-I2C connected sonar
 *	device (SRF008) and returns the the distance between the sonar
 *	sensor and the nearest object in inches.
 *
 *	Note: 	This function will continue executing until valid data
 *			is received from the sensor.
 */
int get_distance(char address)
{
   struct termios camera_term, old_term;
   
   int tty, res, screen, i;
   if(debug)
   	  fprintf(stderr, "Setting up tty device...\n");
   tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

   /*Set serial port settings*/
   tcgetattr(tty, &old_term);
   camera_term.c_cflag = B19200;
   camera_term.c_cflag |= CS8;
   camera_term.c_cflag |= CREAD;
//   camera_term.c_cflag = CSTOPB;
   camera_term.c_iflag = IGNPAR | IGNBRK;
   camera_term.c_cflag |= CLOCAL;
   if(debug)
      fprintf(stderr, "Flushing tty...\n");
   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&camera_term);


	char sbuf[100];
	char rbuf[4];
	int search_address = address;


	char status = -1;
	unsigned int distance=0;
	int j;
    sbuf[100];
    rbuf[4];

    if(debug)
      fprintf(stderr, "Setting gain...\n");
	sbuf[0] = I2C_CMD;			// send gain limit
	sbuf[1] = search_address;
	sbuf[2] = 0x01;
	sbuf[3] = 0x01;
	sbuf[4] = 20;
    write(tty, &sbuf, 5);
    if(debug)
       fprintf(stderr, "Waiting on status...\n");
    status = -1;
    j=0;
	while (status == -1) {
	  read(tty, &status, 1);
    }
    if(debug)
       fprintf(stderr, "\nstatus after gain set is: %d\n", status);
	
    if(debug)
       fprintf(stderr, "Ranging...\n");
	sbuf[0] = I2C_CMD;			// send sonar ranging inches command
	sbuf[1] = search_address;
	sbuf[2] = 0x00;
	sbuf[3] = 0x01;
	sbuf[4] = 0x50;
    status = -1;
    write(tty, &sbuf, 5);
	usleep(70000); //sleep for 70 ms
    while (status == -1) {
      read(tty, &status, 1);
    }
    if(debug)
       fprintf(stderr, "status after range set is: %d\n", status);
	
	usleep(70000);
	
	while (1) {
      
      if(debug)
         fprintf(stderr, "Reading range...\n");
	  sbuf[0] = I2C_CMD;			// receive sonar ranging inches command
	  sbuf[1] = search_address+1;
	  sbuf[2] = 0x02;
	  sbuf[3] = 0x02;
      rbuf[0] = -1;
	  usleep(65000);
	  write(tty,&sbuf,4);
	  usleep(5000);
      while (rbuf[0] == -1) {
         read(tty, &rbuf, 2);
      }
      if(debug)
         fprintf(stderr, "Reading %x rbuf[0] rbuf[1] is: %x %x\n", search_address, rbuf[0], rbuf[1]);
 
      distance = rbuf[0]<<8;
      distance |= rbuf[1];

	  /*Data in this range is invalid.
	    Ranges above 236 are not reliable*/
	  if(distance > 236 || distance < 0){
	  	usleep(10000);
	  }
	  else {
   		tcflush(tty, TCOFLUSH);
		close(tty);
   		return distance;
	  }
	}
}


double get_front_sonar()
{
	double distance_center = 0;
	distance_center = get_distance(0xe2);
	return distance_center;
}

double get_right_sonar()
{
	double distance_right = 0;
	distance_right = get_distance(0xe0);
	return distance_right;
}

int main(int argc, char * argv[])
{
	if(argc > 1){
		fprintf(stderr, "argv[0]: %s\n", argv[0]);
		fprintf(stderr, "argv[1]: %s\n", argv[1]);
		if(*(argv[1] + 1) == 'd')
			debug = 1;
	}
	int distance_center = 0;
	int distance_right = 0;	
	//sleep(1);
	while (1) {
		usleep(100000);
		distance_center = get_distance(0xe2);
		fprintf(stderr,"e2: distance is: %u\n",distance_center);
		distance_right = get_distance(0xe0);
		fprintf(stderr,"e0: distance is: %u\n",distance_right);
	}
	//fprintf(stderr,"e0: distance is: %u\n",get_distance(tty,0xe0));
}
