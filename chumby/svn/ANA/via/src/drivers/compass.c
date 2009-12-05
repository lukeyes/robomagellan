#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define I2CD_CMD  0x53
#define I2C_CMD   0x55
#define CM01_CMD  0x5a


double get_heading()
{
   struct termios camera_term, old_term;
   
   int tty, res, screen, i;
   tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

   /*Set serial port settings*/
   tcgetattr(tty, &old_term);
   camera_term.c_cflag = B19200;
   camera_term.c_cflag |= CS8;
   camera_term.c_cflag |= CREAD;
//   camera_term.c_cflag = CSTOPB;
   camera_term.c_iflag = IGNPAR | IGNBRK;
   camera_term.c_cflag |= CLOCAL;

   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&camera_term);
	char sbuf[100];
	char rbuf[4];
	static int search_address = 0xC0;

	double degree;
	int degInt;
	int j;
	char status = -1;

	while (1){

		sbuf[0] = I2C_CMD;			// receive compass bearing command
		sbuf[1] = search_address+1;
		sbuf[2] = 0x02;
		sbuf[3] = 0x02;
      	rbuf[0] = -1;
//		sbuf[4] = 0x50;
      //sleep(1);
      	write(tty,&sbuf,4);
      	while (rbuf[0] == -1) {
//         	fprintf(stderr, "after read: %d\n", rbuf[0]);
         	read(tty, &rbuf, 2);
        	 //fprintf(stderr, "rbuf[0] is: %d\n", rbuf[0]);
      	}
        	// fprintf(stderr, "rbuf[1] is: %u\n", rbuf[1]);

      	degInt = rbuf[0]<<8;
      	degInt |= rbuf[1];
	  	degree = degInt/10.0;
	  	if (degree < 0 || degree > 359) {
			//fprintf(stderr, "compass is crapping out: %f\n", degree);

   			tcflush(tty, TCOFLUSH);
			close (tty);
   			tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
   			/*Set serial port settings*/
   			tcgetattr(tty, &old_term);
   			camera_term.c_cflag = B19200;
   			camera_term.c_cflag |= CS8;
  			camera_term.c_cflag |= CREAD;
   			camera_term.c_iflag = IGNPAR | IGNBRK;
   			camera_term.c_cflag |= CLOCAL;

   			tcflush(tty, TCOFLUSH);
   			tcsetattr(tty,TCSANOW,&camera_term);
			usleep(50000);
	  	} else {
   			tcflush(tty, TCOFLUSH);
			close (tty);
	  		return degree;
		}
	  }

}

main () {
	double heading;
	while (1) {
		heading = get_heading();
		fprintf(stderr, "heading is: %f\n", heading);
		usleep(50000);
	}
}
