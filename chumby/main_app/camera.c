/* Camera driver
Written by Scott Hollwedel s.hollwedel@gmail.com
*/

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "camera.h"

// Must match camera.h error code structure
const char * camera_err_msgs[NUM_CAMERA_ERRORS] = {
    "NO_ERROR",
    "CAMERA_OPEN_FAIL",
    "CAMERA_WRITE_ERR",
    "CAMERA_READ_ERR",
    "CAMERA_START_TRACK_TIMEOUT",
    "CAMERA_STOP_TRACK_TIMEOUT",
    "CAMERA_CONE_POS_TIMEOUT"
};

static int tty=0;

int camera_init()
{
    struct termios camera_term;
    
	// Initialize serial to camera
	int tty = open("/dev/cam", O_RDWR | O_NOCTTY | O_NONBLOCK);
   	//tcgetattr(tty, &camera_term);
	cfmakeraw(&camera_term);
  	camera_term.c_cflag |= B115200;
   	camera_term.c_cflag |= CS8 | CREAD | CLOCAL;
   	camera_term.c_iflag |= IGNPAR | IGNBRK;
	tcsetattr(tty,TCSAFLUSH,&camera_term);
	usleep(50000);
	return tty;
	
    // If a reinit occurs
    if(tty)
        close(tty);
}

//Start tracking - turn on camera, pass correct parameters, and start spitting out tracking packets
int camera_start_tracking()
{
    char buffer[5];
    
    //Set tracking settings
    //char on[] = "cp 1\r"; //Turn camera on
    char tc[] = "om 0 1\r"; //Suppress output from tracking except middle x
    char servo[] = "sm 0\r";//Turn off servo movement and reporting
    char color_track[] = "tc 144 194 37 87 0 41\r"; //Start color tracking
    int retries = 0;
    
    fprintf(stderr,"camera_start_tracking: 1\n");
    do {
        fprintf(stderr,"camera_start_tracking: 1.1\n");
        fprintf(stderr,"sizeof tc: %d\n", sizeof(tc));
        if(write(tty,&tc,sizeof(tc) - 1) < 0)
            return CAMERA_WRITE_ERR;
        usleep(50000);
        fprintf(stderr,"camera_start_tracking: 1.2\n");
        if(read(tty, buffer, 5) < 0)
            return CAMERA_READ_ERR;
        fprintf(stderr,"camera_start_tracking: 1.3\n");
        if(retries++ > 20)
            return CAMERA_START_TRACK_TIMEOUT; 
        fprintf(stderr,"camera_start_tracking: 1.4\n");
    } while (buffer[0] != 'A');
    
    fprintf(stderr,"camera_start_tracking: 2\n");
    do {
        if(write(tty,&servo,sizeof(servo)) < 0)
            return CAMERA_WRITE_ERR;
        usleep(50000);
        if(read(tty, buffer, 5) < 0)
            return CAMERA_READ_ERR;
        if(retries++ > 20)
            return CAMERA_START_TRACK_TIMEOUT; 
    } while (buffer[0] != 'A');
    
    fprintf(stderr,"camera_start_tracking: 3\n");
    do {
        if(write(tty, &color_track,sizeof(color_track)) < 0)
            return CAMERA_WRITE_ERR;
        usleep(50000);
        if(read(tty, buffer, 5) < 0)
            return CAMERA_READ_ERR;
        if(retries++ > 20)
            return CAMERA_START_TRACK_TIMEOUT; 
    }while(buffer[0] != 'A');

    fprintf(stderr,"camera_start_tracking: 4\n");
    return CAMERA_NO_ERROR;
}

//Reset to default values, turn off camera, and sleep processor
int camera_stop_tracking()
{
    char reset[] = "rs\r";
    char off[] = "cp 0\r";
    char sleep[] = "sl\r";
    char buffer[5];
    int retries = 0;
    do {
        if(write(tty,&off,sizeof(off)) < 0)
            return CAMERA_WRITE_ERR;
        usleep(50000);
        if(read(tty, buffer, 5) < 0)
            return CAMERA_READ_ERR;
        if(retries++ > 20)
            return CAMERA_STOP_TRACK_TIMEOUT; 
    } while (buffer[0] != 'A');
    
    retries=0;

    do {
        if(write(tty,&reset,sizeof(reset)) < 0)
            return CAMERA_WRITE_ERR;
        usleep(50000);
        if(read(tty, buffer, 4) < 0)
            return CAMERA_READ_ERR;
        if(retries++ > 20)
            return CAMERA_STOP_TRACK_TIMEOUT; 
    } while (buffer[0] != 'A');
    
    if(write(tty,&sleep, sizeof(sleep)) < 0)
            return CAMERA_WRITE_ERR;

    tcflush(tty,TCIFLUSH);
    usleep(50000);  
    
    return CAMERA_NO_ERROR;
}


//Value between 46 and 210, 128 is the center
//13 is ASCII return, 32 is spaces
//0 indicates nothing found
//Do a 3 point average

int camera_cone_position()
{
    int position=0;
    int retries=0;
    char buffer[6];//(T mx)
 
    while (!position) {
        //Clear input buffer to ensure we are getting the latest info
        tcflush(tty,TCIFLUSH);
        usleep(150000);//Time delay to allow buffer to fill back up with latest tracking info
        position = 0;
        memset(buffer, 0, sizeof(buffer));

        if(read(tty, buffer, sizeof(buffer)) < 0)
            return CAMERA_READ_ERR;

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
        else if(retries++ > 20)
        {
            return CAMERA_CONE_POS_TIMEOUT;
        }

    }

    return position;
}
