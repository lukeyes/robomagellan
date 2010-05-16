/* Main driver file for usb_i2c module
*  Written by: Scott Hollwedel <arkitect@gmail.com>
*  Modified by: Michael Ortiz <mtortiz.mail@gmail.com>
*/

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

//USB-I2C commands
#define I2C_AD1 0x55 //Read/Write single or multiple bytes for 1 byte addressed
                     //devices (the majority of devices will use this one)
static int tty;

int usb_i2c_init()
{
        struct termios i2c_term;
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

int usb_i2c_write(int address, int reg, int nbytes, int data)
{
        char buf[5];
        buf[0] = I2C_AD1;
        buf[1] = address;
        buf[2] = reg;
        buf[3] = nbytes;
        buf[4] = data;
        write(tty, &buf,sizeof(buf));
        return 0;
}

int usb_i2c_read(int * buf, int size) {
    read(tty, buf, size);
    return 0;
}
