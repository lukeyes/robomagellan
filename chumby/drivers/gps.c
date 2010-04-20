#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

struct Location
{
        double int_lat;
        double int_long;
};

int initialize_gps()
{
  int tty;
  struct termios gps_term;

  tty = open("/dev/gps", O_RDWR | O_NOCTTY);
  tcgetattr(tty, &gps_term);
  cfmakeraw(&gps_term);
  cfsetspeed(&gps_term, B4800);
  gps_term.c_cflag = CS8 | CLOCAL | CREAD;
  gps_term.c_iflag = IGNPAR | IGNBRK;
  gps_term.c_cc[VTIME] = 10;
  gps_term.c_cc[VMIN] = 0;
  tcsetattr(tty,TCSAFLUSH,&gps_term);

  return tty;
}

void convert_latitude(char * lat, struct Location* position)
{
        int count;
        double mult = .0001;
        position->int_lat = 0;
        for(count = 8;count > 4;count--)
        {
                position->int_lat = position->int_lat + ((lat[count]-48)*mult);
                mult = mult*10;
        }
        for(count = 3;count >= 0;count--)
        {
                position->int_lat = position->int_lat + ((lat[count]-48)*mult);
                mult = mult*10;
        }
}

void convert_longitude(char * lon, struct Location* position)
{
        int count;
        double mult = .0001;
        position->int_long = 0;
        for(count = 9;count > 5;count--)
        {
                position->int_long = position->int_long + ((lon[count]-48)*mult);
                mult = mult*10;
        }

        for(count = 4;count >= 0;count--)
        {
                position->int_long = position->int_long + ((lon[count]-48)*mult);
                mult = mult*10;
        }
}

//Read GPS coordinates and return lat and long - data is in NMEA 0183 format
void get_gps(int tty_gps, struct Location* position)
{
        unsigned char buf[38]; //Fill up with each line
        unsigned char data = 0;
        unsigned char latitude[11];
        unsigned char longitude[12];
        int count = 0;
        int found = 0;
       
        //Disregard data until $GPRMC is found
        while( found != 1)
        {
                read(tty_gps, &buf[0], 1);
                if(buf[0] == '$')
                {
                        for(count = 0;count < 5;count++)
                        {
                                read(tty_gps,&buf[count],1);
                        }
                        if(strncmp(buf, "GPRMC", 5) == 0)
                        {      
                                //Read in GPS data and get lat and long
                                found = 1;
                                for(count = 0;count < 37;count++)
                                {
                                        read(tty_gps,&buf[count], 1);
                                }
                                memcpy(latitude, &buf[14],11);
                                memcpy(longitude,&buf[26],12);
                        }
                }
       
        }
        convert_latitude(latitude,position);
        convert_longitude(longitude,position);
}


int main()
{
        struct Location c_position;
        int tty_gps = initialize_gps();
        int count = 0;
        while(count < 6)
        {
                get_gps(tty_gps,&c_position);
                printf("Latitude: %f\n",c_position.int_lat);
                printf("Longitude: %f\n",c_position.int_long);
                count++;
        }
       
        close(tty_gps);
        return 0;
}
