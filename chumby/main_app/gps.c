#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gps.h"
#include "atan.c"

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
}

double calc_target_distance(struct Location* pos, struct Location* dest)
{
    double dlat, dlong, dist;
    dlat = dest->latitude - pos->latitude;
    dlong = dest->longitude - pos->longitude;

	dist = (dlat * dlat) + (dlong * dlong);

    printf("dlat = %f\ndlong = %f\nrise = %f\npolangle = %f\n",dlat,dlong,rise,polangle);

    return dist;
}

double calc_target_heading(struct Location* pos, struct Location* dest)
{
    double dlat, dlong, rise, polangle=0;
    dlat = dest->latitude - pos->latitude;
    dlong = dest->longitude - pos->longitude;
    rise = dlong/dlat;

    polangle = atan(rise) + ((dlong < 0) ? 180 : 0);

    printf("dlat = %f\ndlong = %f\nrise = %f\npolangle = %f\n",dlat,dlong,rise,polangle);

    return polangle;
}

main()
{
	struct Location pos, dest;
	pos.latitude = 32.93154766247661;
	pos.longitude = -110.9234619140625;
	dest.latitude = 32.41366566526167;
	dest.longitude = -110.2862548828125;

	printf("From %f,%f to %f,%f:  at heading %f\n",pos.latitude,pos.longitude,dest.latitude,dest.longitude,calc_target_heading(&pos,&dest));
}
