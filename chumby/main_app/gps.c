#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "gps.h"

static int tty;

void convert_latitude(char * lat, struct Location* position);
void convert_longitude(char * lon, struct Location* position);
double convert_nmea_ll(char * lat);

int gps_init()
{
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

double convert_nmea_ll(char * lat)
{
    int degrees = 0;
    double minutes;

    int count;
    int dot = 0;

    while (lat[dot] != '.')
    {
        dot++;
    }

    for ( count = 0; count < dot - 2; count++ )
    {
        degrees *= 10;
        degrees += lat[count] - '0';
    }

    minutes = (10 * lat[dot - 2]) + lat[dot - 1];
    minutes += (.1 * lat[dot + 1]) + (.01 * lat[dot+2]);

    return degrees + (minutes / 60);
}

void convert_latitude(char * lat, struct Location* position)
{

    int count;
    double mult = .0001;
    position->latitude = 0;
    for(count = 8;count > 4;count--)
    { 
        position->latitude += ((lat[count]-48)*mult); 
        mult = mult*10;
    }
    for(count = 3;count >= 0;count--)
    {
        position->latitude += ((lat[count]-48)*mult); 
        mult = mult*10;
    }
}

void convert_longitude(char * lon, struct Location* position)
{
    int count;
    double mult = .0001;
    position->longitude = 0;
    for(count = 9;count > 5;count--)
    { 
        position->longitude += ((lon[count]-48)*mult); 
        mult = mult*10;
    }

    for(count = 4;count >= 0;count--)
    {
        position->longitude += ((lon[count]-48)*mult); 
        mult = mult*10;
    }
}

//Read GPS coordinates and return lat and long - data is in NMEA 0183 format
void gps_get_position(struct Location* position)
{
    unsigned char buf[38]; //Fill up with each line
    unsigned char latitude[11];
    unsigned char longitude[12];
    int count = 0;
    int found = 0;
    
    //Disregard data until $GPRMC is found and is valid
    while( found != 1)
    {
        read(tty, &buf[0], 1);
        if(buf[0] == '$')
        {
            for(count = 0;count < 5;count++)
            {
                read(tty,&buf[count],1);
            }
            if(strncmp((char *)buf, "GPRMC", 5) == 0)
            {   
                //Read in GPS data and get lat and long
                found = 1;
                for(count = 0;count < 37;count++)
                {
                    read(tty,&buf[count], 1);
                }
                if(buf[12] == 'V');
                    found = 0;
                memcpy(latitude, &buf[14],11);
                memcpy(longitude,&buf[26],12);
            }
        }
    
    }
    position->latitude = convert_nmea_ll((char *)latitude);
    position->longitude = convert_nmea_ll((char *)longitude);
    //convert_latitude(latitude,position);
    //convert_longitude(longitude,position);
}

double calc_target_heading(struct Location* pos, struct Location* dest)
{
    double dlat, dlong, rise, polangle=0;
    dlat = dest->latitude - pos->latitude;
    dlong = dest->longitude - pos->longitude;
    rise = dlong/dlat;

    //polangle = atan(rise);
    printf("dlat = %f\ndlong = %f\nrise = %f\npolangle = %f\n",dlat,dlong,rise,polangle);

    if (dlong < 0)
    {
        polangle += 180;
    }

    return 90 - polangle;
}
