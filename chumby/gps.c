#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "main.h"

int TTY;

int open_gps() {
  struct termios gps_term;

  TTY = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
  tcgetattr(TTY, &gps_term);

  gps_term.c_cflag = B4800;
  gps_term.c_cflag |= CS8;
  gps_term.c_cflag |= CREAD;
  gps_term.c_iflag = IGNPAR | IGNBRK;
  gps_term.c_cflag |= CLOCAL;
  tcsetattr(TTY,TCSANOW,&gps_term);
  tcflush(TTY, TCOFLUSH);
  tcflush(TTY, TCIFLUSH);
  return TTY;
}

void ReadData(double data[], int TTY) {
   char buf[BUFSIZ];
   char word[7][50];
   char * pStr;
   char * tmp;
   int i;
   int res;
   /*Variables needed from $GPRMC string*/
   char status[BUFSIZ];
   float longitude;
   //char long_direction;
   float latitude;
   //char lat_direction;
   float bearing;
   float current_direction;

   res = read(TTY, buf, BUFSIZ);

   if(buf[0]=='$' && buf[1]=='G' && buf[2]=='P' && buf[3]=='R' && buf[4]=='M' && buf[5]=='C')
   {
      for(i=0;buf[i]!='\0';i++)
      {
         if(buf[i]==',')
            buf[i] = ' ';
      }

      sscanf(buf,"%*s %*s %s %f %*s %f %*s %f %f %*s %*s %*s %*s",status,&latitude,&longitude,&bearing,&current_direction);

      data[0] = latitude;
      data[1] = longitude;
//      printf("%s --- %f --- %f --- %f --- %f\n",status,latitude,longitude,bearing,current_direction);

//      printf("%s", buf);
//     sleep(1);
   }
}

double Delta(double a, double b)
{
        return a - b;
}

double OppositeAngle(double angle)
{
        double retVal = 0.0;

        if (angle <= 180.0)
                retVal = angle + 180.0;
        else if (angle > 180.0)
                retVal = angle - 180.0;

        return retVal;
}

int CalcQuadrant(double coneLat, double coneLong, double carLat, double carLong)
{
        double deltaX, deltaY;

        deltaX = Delta(coneLong, carLong);
        deltaY = Delta(coneLat, carLat);

        // quad0 is upper right, quad1 is lower right
        // quad2 is lower left, quad3 is upper left
        if (deltaX > 0)
        {
                if (deltaY < 0)
                        return QUAD0;
                else
                        return QUAD1;
        }
        else
        {
                if (deltaY < 0)
                        return QUAD3;
                else
                        return QUAD2;
        }
}

double CalcNRef(int quadrant, double coneLat, double coneLong, double carLat,
                double carLong)
{
        double deltaX, deltaY;
        double angle;

        deltaX = Delta(coneLong, carLong);
        deltaY = Delta(coneLat, carLat);

        if (quadrant == QUAD3)
        {
                deltaX = -deltaX;
                deltaY = -deltaY;
        }
        else if (quadrant == QUAD2)
        {
                deltaX = -deltaX;
        }
        else if (quadrant == QUAD0)
                deltaY = -deltaY;

        //printf("deltaX: %f,  deltaY: %f\n", deltaX, deltaY);
        angle = atan(deltaY / deltaX);
        return 90 - (180 / (M_PI)) * angle;
}

int Turn(int quadrant, double nRef, double heading)
{
        double refLineAngle = 0.0; // angle from North to line containing robot and cone (counter clockwise)
        double headingMinusRefAngle = 0.0; // compass heading minus refLineAngle wrt North
        double turnAngle = 0.0;
        int turnDirection = 0; // 1 is right, 0 is left
        int retVal = 115; // 128 - center line; 64 - half turn left;
        // 192 - half turn right;  0 - full turn left;  255 - full turn right
        int C_180 = 180;
        double turnAngleMinus180;

        if ((quadrant == QUAD3) || (quadrant == QUAD1))
                refLineAngle = (90 * quadrant) + (90 - nRef);
        else if ((quadrant == QUAD2) || (quadrant == QUAD0))
                refLineAngle = (90 * quadrant) + nRef;

        /*printf("refLineAngle: %f\n", refLineAngle);*/
        if (heading < OppositeAngle(refLineAngle))
                headingMinusRefAngle = heading - refLineAngle;
        else if (heading >= OppositeAngle(refLineAngle))
                headingMinusRefAngle = refLineAngle - heading;

        if (headingMinusRefAngle >= 0)
        {
                if ((quadrant == QUAD3) || (quadrant == QUAD2))
                        turnDirection = 0;
                else if ((quadrant == QUAD1) || (quadrant == QUAD0))
                        turnDirection = 1;

                turnAngle = fabs(C_180 - headingMinusRefAngle);
        }
        else if (headingMinusRefAngle < 0)
        {
                if ((quadrant == QUAD3) || (quadrant == QUAD2))
                        turnDirection = 1;
                else if ((quadrant == QUAD1) || (quadrant == QUAD0))
                        turnDirection = 0;

                turnAngle = fabs(C_180 + headingMinusRefAngle);
        }

        // need to return a number [0, 255]
        // 125-130 means don't turn (close to 128, which is the center line)

        printf("turnDirection: %d  turnAngle: %f\n", turnDirection, turnAngle);

        turnAngleMinus180 = fabs(turnAngle - C_180);
        if (turnDirection == 0)
        {
                if ((turnAngleMinus180 >= 0) && (turnAngleMinus180 < TURN_MAX))
                        retVal = 110;
                else if ((turnAngleMinus180 >= TURN_MAX) && (turnAngleMinus180 < TURN_HIG))
                        retVal = 112;
                else if ((turnAngleMinus180 >= TURN_HIG) && (turnAngleMinus180 < TURN_LOW))
                        retVal = 113;
                else if ((turnAngleMinus180 >= TURN_LOW) && (turnAngleMinus180 < TURN_MIN))
                        retVal = 114;
                else
                        retVal = 115;
        }
        else if (turnDirection == 1)
        {
                if ((turnAngleMinus180 >= 0) && (turnAngleMinus180 < TURN_MAX))
                        retVal = 120;
                else if ((turnAngleMinus180 >= TURN_MAX) && (turnAngleMinus180 < TURN_HIG))
                        retVal = 119;
                else if ((turnAngleMinus180 >= TURN_HIG) && (turnAngleMinus180 < TURN_LOW))
                        retVal = 118;
                else if ((turnAngleMinus180 >= TURN_LOW) && (turnAngleMinus180 < TURN_MIN))
                        retVal = 117;
                else
                        retVal = 115;
        }
        return retVal;
}

/* -----------------------------------------------------------------------------
 * Function: DirectionTo()
 * Purpose: To find the direction to a certain location given the point to go
 *      to and the point we're at.
 * Description: Copied from the GPS code.
 * Parameters:
 *  - carLat: Our latitude.
 *  - carLong: Our longitude.
 *  - coneLat: The latitude we want to get to.
 *  - coneLong: The longitude we want to get to.
 * Return value: Returns the direction we need to go to.
 * -------------------------------------------------------------------------- */
double DirectionTo (double carLat, double carLong, double coneLat,
        double coneLong) {
        int quadrant = CalcQuadrant (carLat, carLong, coneLat, coneLong);
        double nRef = CalcNRef (quadrant, carLat, carLong, coneLat, coneLong);
        double refLineAngle = 0.0;  // angle from North to line containing robot and cone

        if ((quadrant == QUAD3) || (quadrant == QUAD1))
                refLineAngle = (90 * quadrant) + (90 - nRef);
        else if ((quadrant == QUAD2) || (quadrant == QUAD0))
                refLineAngle = (90 * quadrant) + nRef;

        while (refLineAngle >= 360)
                refLineAngle -= 360;
        while (refLineAngle < 0)
                refLineAngle += 360;

        return refLineAngle;
}

/* -----------------------------------------------------------------------------
 * Function: DistanceBetween()
 * Purpose: To calculate the distance between two GPS coordinates.
 * Description: Uses the standard distance formula, converts to inches as well.
 * Parameters:
 *      - coord1Lat: Latitude of first coordinate.
 *  - coord1Long: Longitude of first coordinate.
 *  - coord2Lat: Latitude of second coordinate.
 *  - coord2Long: Longitude of second coordinate.
 * Return value: Returns the distance, in inches, between the two coordinates.
 * -------------------------------------------------------------------------- */
double DistanceBetween (double coord1Lat, double coord1Long, double coord2Lat,
        double coord2Long) {
        double deltaLat = coord1Lat - coord2Lat;
        double deltaLong = coord1Long - coord2Long;
        deltaLat *= INCHES_PER_DEG_LAT;
        deltaLong *= INCHES_PER_DEG_LONG;
        return sqrt(deltaLat*deltaLat + deltaLong*deltaLong);
}
int get_turn_value (double coneLat, double coneLong,int tty,double carLat, double carLong)
{
        double heading = 0;// strtod(argv[5], NULL);
        int quadrant;
        double nref;
        int turnValue;

        quadrant = CalcQuadrant(coneLat, coneLong, carLat, carLong);
        nref = CalcNRef(quadrant, coneLat, coneLong, carLat, carLong);
        heading = get_heading();
        turnValue = Turn(quadrant, nref, heading);
        return turnValue;
}
