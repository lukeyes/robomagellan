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

const char * camera_err_msgs[NUM_CAMERA_ERRORS] = {
    "NO_ERROR",
    "CAMERA_OPEN_FAIL",
    "CAMERA_WRITE_ERR",
    "CAMERA_READ_ERR",
    "CAMERA_START_TRACK_TIMEOUT",
    "CAMERA_STOP_TRACK_TIMEOUT",
    "CAMERA_CONE_POS_TIMEOUT",
};

static int tty;

int camera_init()
{
    struct termios camera_term;

    // If a reinit occurs
    if(tty > 0)
        close(tty);

    // Initialize serial to camera
    tty = open("/dev/cam", O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(tty < 0)
        return CAMERA_OPEN_FAIL;

    tcgetattr(tty, &camera_term);
    cfmakeraw(&camera_term);
    camera_term.c_cflag = B115200;
    tcsetattr(tty,TCSAFLUSH,&camera_term);
    usleep(50000);
    return CAMERA_NO_ERROR;
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

    return 0;
}

//Reset to default values, turn off camera, and sleep processor
int camera_stop_tracking()
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

    return 0;
}


//Value between 46 and 210, 128 is the center
//13 is ASCII return, 32 is spaces
//0 indicates nothing found
//Do a 3 point average

int camera_cone_position()
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
        position = camera_cone_position();
    }

    return position;
}

void camera_close() {
    close(tty);
}
