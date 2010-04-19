//GPS NMEA Driver + Comparitive positioning computation
////    Written by Matthew Rasmussen, 2010
////    Released under GPL license.
////            !!When compiling for Chumby, use Lib Math ("-lm" flag) or       !!
////            !!it won't work!                                                !!

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "gps.h"

#define PI 3.14159265358979
//Radius in meters:

#define R               6378000
#define DtoR    (PI / 180)
#define RtoD    (180 / PI)

static int tty;

void convert_latitude(char * lat, struct Location* position);
void convert_longitude(char * lon, struct Location* position);
double convert_nmea_ll(char * lat);

int gps_init()
{
  struct termios gps_term;

  //tty = open("/dev/gps", O_RDWR | O_NOCTTY);
  tty = open("/dev/tty.PL2303-0000201A", O_RDWR | O_NOCTTY);
  tcgetattr(tty, &gps_term);
  cfmakeraw(&gps_term);
  cfsetspeed(&gps_term, B4800);
  gps_term.c_cflag = CS8 | CLOCAL | CREAD;
  gps_term.c_iflag = IGNPAR | IGNBRK;
  gps_term.c_lflag = ICANON;
  gps_term.c_cc[VTIME] = 10;
  gps_term.c_cc[VMIN] = 0; 
  tcsetattr(tty,TCSAFLUSH,&gps_term);

  fprintf(stderr,"tty opened:%d\n",tty);
  return tty;
}

double convert_nmea_ll(char * lat)
{
    int degrees = 0;
    double minutes;

    int count;
    int dot = 0;
	double a = 1;

    while (lat[dot] != '.')
    {
        dot++;
    }

    for ( count = 0; count < dot - 2; count++ )
    {
        degrees *= 10;
        degrees += lat[count] - '0';
    }

    minutes = (10 * (lat[dot - 2] - '0')) + (lat[dot - 1] - '0');
	for (count = dot + 1; lat[count] <= '9' && lat[count] >= '0'; count++)
	{
		a *= 0.1;
		minutes += (lat[count] - '0') * a;
	}

    return (degrees + (minutes / 60)) * ((lat[count+1] == 'W' || lat[count+1] == 'S') ? -1 : 1);
}

//Read GPS coordinates and return lat and long - data is in NMEA 0183 format
int gps_get_position(struct Location* position)
{
    unsigned char buf[64]; //Fill up with each line
    unsigned char latitude[11];
    unsigned char longitude[12];
    int found = 0;
	int retries = 0;
	int messages = 0;
    
    //Disregard data until $GPRMC is found and is valid
    while( found != 1)
    {
		while (read(tty, &buf[0], 64) <= 0)
		{
			fprintf(stderr,"Waiting on successful read, retries: %d\n",retries);
			usleep(5000);
			if(retries++ > 50)
				return 0;
		}

		fprintf(stderr,"%d messages processed, found: %d\n",messages,found);
        if(strncmp((char *)buf, "$GPRMC", 6) == 0)
        {
            //Read in GPS data and get lat and long
            found = 1;
            if(buf[18] == 'V');
                found = 0;
            memcpy(latitude, &buf[20],11);
            memcpy(longitude,&buf[32],12);
        }
		messages++;
    }
    position->latitude = convert_nmea_ll((char *)latitude);
    position->longitude = convert_nmea_ll((char *)longitude);

	return 1;
}

double calc_target_distance(struct Location* pos, struct Location* dest)
{
    double a1,b1,a2,b2,dist;

        a1 = pos->latitude * DtoR;
        b1 = pos->longitude * DtoR;
        a2 = dest->latitude * DtoR;
        b2 = dest->longitude * DtoR;

        dist = acos((cos(a1) * cos(b1) * cos(a2) * cos(b2)) + (cos(a1) * sin(b1) * cos(a2) * sin(b2)) + (sin(a1) * sin(a2))) * R;

        printf("a1: %f\nb1: %f\na2: %f\nb2: %f\n",a1,b1,a2,b2);
    printf("distance: %f meters.\n",dist);

    return dist;
}

double calc_target_heading(struct Location* pos, struct Location* dest)
{
    double dlong, lat1, lat2, y, x, bearing;

        lat1 = pos->latitude * DtoR;
        lat2 = dest->latitude * DtoR;
        dlong = (dest->longitude - pos->longitude) * DtoR;
        
        y = sin(dlong) * cos(lat2);
        x = (cos(lat1) * sin(lat2)) - (sin(lat1) * cos(lat2) * cos(dlong));
        bearing = atan2(y,x) * RtoD;
        bearing = (bearing < 0) ? bearing + 360 : bearing;

    return bearing;
}

#if 1
main()
{
        struct Location pos, dest;
        dest.latitude = 32.881136091348665;
        dest.longitude = -117.23563715815544;

        while (1)
        {
			if (gps_get_position(&pos) < 1)
				printf("Error reading.\n");
            gps_get_position(&pos);
            printf("From %f,%f to %f,%f: %f  at heading %f\n",pos.latitude,pos.longitude,dest.latitude,dest.longitude,calc_target_distance(&pos,&dest),calc_target_heading(&pos,&dest));
        }
}
#endif
