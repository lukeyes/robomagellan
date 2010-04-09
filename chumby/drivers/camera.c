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
	int tty = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NONBLOCK);
   	tcgetattr(tty, &camera_term);
	cfmakeraw(&camera_term);
  	camera_term.c_cflag = B115200;
   	camera_term.c_cflag = CS8 | CREAD | CLOCAL;
   	camera_term.c_iflag = IGNPAR | IGNBRK;
	tcsetattr(tty,TCSANOW,&camera_term);
	usleep(50000);
	return tty;
}

//Start tracking - turn on camera, pass correct parameters, and start spitting out tracking packets
void start_tracking(int tty)
{
	char buffer[5];
	
	//Set tracking settings
	char on[] = "cp 1\r"; //Turn camera on
	char tc[] = "om 0 64\r"; //Suppress output from tracking except pixels out
   	char servo[] = "sm 5\r";//Set to pan servo active and pan servo report
	char color_track[] = "tc 144 194 37 87 0 41\r"; //Start color tracking
	
	do {
		write(tty,&on,sizeof(on));
		usleep(50000);
		read(tty, buffer, 5);
	} while (buffer[0] != 'A');
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
void stop_tracking(int tty)
{
	char reset[] = "rs \r";
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
int servo_position(int tty)
{
	//Clear input buffer to ensure we are getting the latest info
	tcflush(tty,TCIFLUSH);
	usleep(150000);//Time delay to allow buffer to fill back up with latest tracking info
	
	int pixels;
	int position;
	int pixelplaces = 0; 
   	char buffer[10];//(T XXX )
	read(tty,buffer,10);
	
	//Convert pixel count to int 
	if(buffer[5] == 32)
	{
		pixels = (buffer[2] - 48)*100 + (buffer[3] - 48)*10 + buffer[4] - 48;
		pixelplaces = 3;
	}
	else if(buffer[4] == 32)
	{
		pixels = (buffer[2] - 48)*10 + buffer[3] - 48;
		pixelplaces= 2;
	}
	else if(buffer[3] == 32)
	{
		pixels = buffer[2] - 48;
		pixelplaces = 1;
	}
	
	//Convert servo direction to int
	if(buffer[5 + pixelplaces] == 13)
	{
		position = (buffer[3 + pixelplaces] - 48) * 10 + buffer[4 + pixelplaces] - 48;
	}
	else if(buffer[6+pixelplaces] == 13)
	{
		position = (buffer[3 + pixelplaces] - 48)*100 + (buffer[4 + pixelplaces] - 48)*10 + buffer[5 + pixelplaces] - 48;
	}
	
	if(pixels > 0);
		return position;
		
	return 0;
}

int main()
{
	//Initialize
	int tty = initialize_camera();
	start_tracking(tty);
	char buffer[] = "sv 0 80\r";
	write(tty, buffer, sizeof(buffer));
	sleep(2);
	printf("%d\n", servo_position(tty));
	
	//camera_scan(tty);
	/*//Start Tracking
	start_tracking(tty);
	
	//Get position
	int count;
	while(1)
	{
		servo_position(tty, &ct);
		printf("Direction: %d\n",ct.location);
		printf("Size: %d\n",ct.size);
	}
	*/
	stop_tracking(tty);
	close(tty);
	return 0;
}
