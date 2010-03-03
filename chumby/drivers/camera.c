#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int initialize_camera()
{
	struct termios camera_term;

	// Initialize serial to camera
	int tty = open("/dev/camera", O_RDWR | O_NOCTTY | O_NDELAY);
   	tcgetattr(tty, &camera_term);
	cfmakeraw(&camera_term);
  	camera_term.c_cflag = B115200;
   	camera_term.c_cflag = CS8 | CREAD | CLOCAL;
   	camera_term.c_iflag = IGNPAR | IGNBRK;
	tcflush(tty, TCOFLUSH);
	tcsetattr(tty,TCSANOW,&camera_term);

	//Set tracking settings
	char tc[] = "om 0 0\r"; //Suppress output from tracking
   	char servo[] = "sm 5\r";//Set to pan servo active and pan servo report
	char color_track[] = "tc 144 194 37 87 0 41\r";
	write(tty,&tc,sizeof(tc));
	rx_ack(tty);
	write(tty,&servo,sizeof(servo));
	rx_ack(tty);
	write(tty, &color_track,sizeof(color_track));
	rx_ack(tty);

	return tty;
}

//Reads in the (ACK + /n + :) from each write command, returns 0 if ACK recieved, -1 if something else
int rx_ack(int tty)
{
	char buffer[5];
	int count;
	for(count = 0; count < sizeof(buffer); count++)
	{
		read(tty,&buffer[count],1);
	}

	if( buffer[0] == 'A')
		return 0;
	else
		return -1;
}

//Turn on servo
void start_tracking(int tty)
{
	char servo[] = "so 0 1\r";//Turn on Servo
	write(tty,&servo,sizeof(servo));
	rx_ack(tty);
}

//Turn off servos
void stop_tracking(int tty)
{
	char servo[] = "so 0 0\r";//Turn off Servo
	write(tty,&servo,sizeof(servo));
	rx_ack(tty);
}


//Value between 46 and 210, 128 is the center
int servo_position(int tty)
{
   	char buffer[9];//(ACK XXX :) NOTE: Not sure if XX is also possible
	int count;
	int position = 0; 
   	char get_servo_pos[] = "gs 0\r"; //Get position of servo 0
	write(tty,&get_servo_pos,sizeof(get_servo_pos));
	for(count = 0; count < sizeof(buffer);count++)
	{
		read(tty,&buffer[count],1);
	}

	position = (buffer[5] - 48)*100 + (buffer[6] - 48)*10 + (buffer[7] - 48);

	return position;
}
