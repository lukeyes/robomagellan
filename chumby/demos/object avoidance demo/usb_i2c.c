/* Main driver file for usb_i2c module
*  Written by: Scott Hollwedel <arkitect@gmail.com>
*/

#include <termios.h>
#include <fcntl.h>
#include "usb_i2c.h"
#define I2C_CMD 0x55

int initialize_i2c()
{
	struct termios i2c_term;
	int tty;
	tty = open("/dev/i2c", O_RDWR | O_NOCTTY );

   	/*Set serial port settings*/
	tcgetattr(tty,&i2c_term);
	cfmakeraw(&i2c_term);
	cfsetspeed(&i2c_term, B19200);
	i2c_term.c_cflag |= (CLOCAL | CREAD);
	i2c_term.c_cflag |= CSTOPB; //2 Stop bits
	i2c_term.c_cflag &= ~PARENB;//No parity
	i2c_term.c_cflag &= ~CSIZE;
   	i2c_term.c_cflag |= CS8;//8 bits
	i2c_term.c_cc[VTIME] = 10;
	i2c_term.c_cc[VMIN] = 0;  			
        tcsetattr(tty, TCSAFLUSH, &i2c_term);			
	
	return tty;	
}

int write_i2c(int tty, int address, int reg, int nbytes, int data)
{
	char buf[5];
	buf[0] = I2C_CMD;
	buf[1] = address;
	buf[2] = reg;
	buf[3] = nbytes;
	buf[4] = data;
	write(tty, &buf,sizeof(buf));
	return 0;
}
