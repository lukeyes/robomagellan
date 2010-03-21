#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int initialize_camera()
{
	struct termios camera_term;
	char buffer[5];

	// Initialize serial to camera
	int tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
   	tcgetattr(tty, &camera_term);
	//cfmakeraw(&camera_term);
  	camera_term.c_cflag = B115200;
   	//camera_term.c_cflag = CS8 | CREAD | CLOCAL;
   	//camera_term.c_iflag = IGNPAR | IGNBRK;
	tcsetattr(tty,TCSANOW,&camera_term);
	usleep(50000);

	//Set tracking settings
	char tc[] = "om 0 0\r"; //Suppress output from tracking
   	char servo[] = "sm 5\r";//Set to pan servo active and pan servo report
	char color_track[] = "st 144 194 37 87 0 41\r"; //Set color tracking settings
	
	do {
		write(tty,&tc,sizeof(tc));
		usleep(50000);
		read(tty, buffer, 5);
	} while (buffer[0] != 'A');
	
	do {
		write(tty,&servo,sizeof(servo));
		usleep(50000);
		read(tty, buffer, 5);
	} while (buffer[0] != 'A');
	
	do {
		write(tty, &color_track,sizeof(color_track));
		usleep(50000);
		read(tty, buffer, 5);
	}while(buffer[0] != 'A');

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
}

//Turn off servos
void stop_tracking(int tty)
{
	char servo[] = "so 0 0\r";//Turn off Servo
	write(tty,&servo,sizeof(servo));
}


//Value between 46 and 210, 128 is the center
int servo_position(int tty)
{
   	char buffer[9];//(ACK XXX :) NOTE: Not sure if XX is also possible
	int count;
	int position = 0; 
   	char get_servo_pos[] = "gs 0\r"; //Get position of servo 0
	write(tty,&get_servo_pos,sizeof(get_servo_pos));
	usleep(50000);
	read(tty,buffer,9);
	if(buffer[6] == 13)
	{
		position = (buffer[4] - 48)*10 + buffer[5] - 48;
	}
	else
	{
		position = (buffer[4] - 48)*100 + (buffer[5] - 48)*10 + (buffer[6] - 48);
	}
	
	return position;
}

int main()
{
	int tty = initialize_camera();
	printf("%d\n",servo_position(tty));
	//char buffer[] = "sv 0 90\r";
	//char input[5];
	//write(tty, buffer, sizeof(buffer));
	//usleep(50000);
	//read(tty,input,5);
	close(tty);
	return 0;
}
