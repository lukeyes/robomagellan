/* Camera driver
Written by Scott Hollwedel s.hollwedel@gmail.com
*/

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
	int tty = open("/dev/cam", O_RDWR | O_NOCTTY | O_NONBLOCK);
   	//tcgetattr(tty, &camera_term);
	cfmakeraw(&camera_term);
  	camera_term.c_cflag = B115200;
   	//camera_term.c_cflag = CS8 | CREAD | CLOCAL;
   	//camera_term.c_iflag = IGNPAR | IGNBRK;
	tcsetattr(tty,TCSAFLUSH,&camera_term);
	usleep(50000);
	return tty;
}

//Start tracking - turn on camera, pass correct parameters, and start spitting out tracking packets
void camera_start_tracking(int tty)
{
	char buffer[5];
	
	//Set tracking settings
	//char on[] = "cp 1\r"; //Turn camera on
	char tc[] = "om 0 1\r"; //Suppress output from tracking except middle x
   	char servo[] = "sm 0\r";//Turn off servo movement and reporting
	char color_track[] = "tc 144 194 37 87 0 41\r"; //Start color tracking
	
	/*do {
		write(tty,&on,sizeof(on));
		usleep(50000);
		read(tty, buffer, 5);
	} while (buffer[0] != 'A');*/
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
}

//Reset to default values, turn off camera, and sleep processor
void camera_stop_tracking(int tty)
{
	char reset[] = "rs\r";
	char off[] = "cp 0\r";
	char sleep[] = "sl\r";
	char buffer[5];
	do {
		write(tty,&off,sizeof(off));
		usleep(50000);
		read(tty, buffer, 5);
	} while (buffer[0] != 'A');
	
	do {
		write(tty,&reset,sizeof(reset));
		usleep(50000);
		read(tty, buffer, 4);
	} while (buffer[0] != 'A');
	
	write(tty,&sleep, sizeof(sleep));
	tcflush(tty,TCIFLUSH);
	usleep(50000);	
}


//Value between 46 and 210, 128 is the center
//13 is ASCII return, 32 is spaces
//0 indicates nothing found
//Do a 3 point average

int camera_cone_position(int tty)
{
	//Clear input buffer to ensure we are getting the latest info
	tcflush(tty,TCIFLUSH);
	usleep(150000);//Time delay to allow buffer to fill back up with latest tracking info
	
	int position;
	int count;
	char buffer[6];//(T mx)
 
   	for(count=0;count<5;count++)
   	{
   		buffer[count] = 0;
   	}
	read(tty,buffer,6);
	if(buffer[3] == 13)
	{
		position = buffer[2] - 48; 
	} 
	else if(buffer[4] == 13)
	{
		position = (buffer[2] - 48)*10+(buffer[3] - 48);
	}
	else if(buffer[5] == 13)
	{
		position = (buffer[2]-48)*100 + (buffer[3]-48) * 10 + (buffer[4]-48);
	}
	else
	{
		position = camera_cone_position(tty);
	}

	return position;
}

