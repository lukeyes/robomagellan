#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "main.h"

/*int main()
{
   struct termios gps_term, old_term;
   char turn0, turn1, turn2, turn3, turn4, turn5, turn6, turn7, turn8, turn9;
   char speed0, speed1, speed2, speed3, speed4, speed5, speed6, speed7, speed8, speed9;
   char back0, back1;

   speed0 = '0';
   speed1 = '1';
   speed2 = '2';
   speed3 = '3';
   speed4 = '4';
   speed5 = '5';
   speed6 = '6';
   speed7 = '7';
   speed8 = '8';
   speed9 = '9';
   back0 = '`';
   back1 = '~';

   turn0 = 'a'; //far left
   turn1 = 'b';
   turn2 = 'c';
   turn3 = 'd';
   turn4 = 'e'; //middle
   turn5 = 'f';
   turn6 = 'g';
   turn7 = 'h';
   turn8 = 'i'; //far right




   int tty, res, screen, i;
   tty = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);

   tcgetattr(tty, &old_term);
   gps_term.c_cflag = B9600;
   gps_term.c_cflag |= CS8;
   gps_term.c_cflag |= CREAD;
   gps_term.c_iflag = IGNPAR | IGNBRK;
   gps_term.c_cflag |= CLOCAL;
   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&gps_term);

   write(tty,&speed0,1);
      sleep(1);
   write(tty,&speed1,1);
      sleep(1);
   write(tty,&speed2,1);
      sleep(1);
   write(tty,&speed3,1);
      sleep(1);
   write(tty,&speed4,1);
      sleep(1);
   write(tty,&speed5,1);
      sleep(1);
   write(tty,&speed6,1);
      sleep(1);
   write(tty,&speed7,1);
      sleep(1);
   write(tty,&speed8,1);
      sleep(1);
   write(tty,&speed9,1);
      sleep(1);
   write(tty,&back0,1);
      sleep(1);
   write(tty,&back1,1);

//   sleep(3);

//   write(tty,&turn0,1);
//      sleep(2);
//   write(tty,&turn1,1);
//    sleep(2);
//   write(tty,&turn2,1);
//      sleep(2);
   write(tty,&turn3,1);
      sleep(2);
   write(tty,&turn4,1);
      sleep(2);
   write(tty,&turn5,1);
      sleep(2);
//   write(tty,&turn6,1);
//      sleep(2);
//   write(tty,&turn7,1);
//      sleep(2);
//   write(tty,&turn8,1);
//      sleep(2);
//   write(tty,&turn9,1);
//      sleep(2);
   write(tty,&turn4,1);
      sleep(2);

   tcsetattr(tty,TCSANOW,&gps_term);
   close(tty);
}
*/
void send_pwm_steer(char value)
{
   int tty;
   struct termios pwm_steer_term, old_term;

   tty = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);

   tcgetattr(tty, &old_term);
   pwm_steer_term.c_cflag = B9600;
   pwm_steer_term.c_cflag |= CS8;
   pwm_steer_term.c_cflag |= CREAD;
   pwm_steer_term.c_iflag = IGNPAR | IGNBRK;
   pwm_steer_term.c_cflag |= CLOCAL;
   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&pwm_steer_term);

   write(tty,&value,1);
   tcsetattr(tty,TCSANOW,&pwm_steer_term);
   close(tty);
}

void send_pwm_motor(char value)
{
   int tty;
   struct termios pwm_motor_term, old_term;

   tty = open("/dev/ttyACM1", O_RDWR | O_NOCTTY | O_NDELAY);

   tcgetattr(tty, &old_term);
   pwm_motor_term.c_cflag = B9600;
   pwm_motor_term.c_cflag |= CS8;
   pwm_motor_term.c_cflag |= CREAD;
   pwm_motor_term.c_iflag = IGNPAR | IGNBRK;
   pwm_motor_term.c_cflag |= CLOCAL;
   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&pwm_motor_term);

   write(tty,&value,1);
   tcsetattr(tty,TCSANOW,&pwm_motor_term);
   close(tty);
}

/* -----------------------------------------------------------------------------
 * Function: SteerTo()
 * Purpose: To determine in what direction we should steer to get to the
 *      intended direction.
 * Description: Taken from "Algorithms reference" page of RoboMagellan_2008
 *      Google Group.
 * Parameters:
 *      - myDirection: The direction the robot's currently facing.
 *  - desiredDirection: The direction we want the robot to be facing.
 * Return value: A character value representing how much to steer, and in which
 *      direction.
 * -------------------------------------------------------------------------- */
char SteerTo (double myDirection, double desiredDirection) {
        double counterClockwiseAngle;
        double clockwiseAngle;
        char steering = STEER_NEUTRAL;

        if (myDirection < desiredDirection) {
            counterClockwiseAngle = desiredDirection - myDirection;
            clockwiseAngle = 2*M_PI - counterClockwiseAngle;
        }
        else {
            clockwiseAngle = myDirection - desiredDirection;
            counterClockwiseAngle = 2*M_PI - clockwiseAngle;
        }

        // Turn right (clockwise).
        if (clockwiseAngle < counterClockwiseAngle) {
                if ((clockwiseAngle >= 0) && (clockwiseAngle < TURN_MAX))
                        steering = STEER_RGHT_MAX;
                else if ((clockwiseAngle >= TURN_MAX) && (clockwiseAngle < TURN_HIG))
                        steering = STEER_RGHT_3;
                else if ((clockwiseAngle >= TURN_HIG) && (clockwiseAngle < TURN_LOW))
                        steering = STEER_RGHT_2;
                else if ((clockwiseAngle >= TURN_LOW) && (clockwiseAngle < TURN_MIN))
                        steering = STEER_RGHT_1;
                else
                        steering = STEER_NEUTRAL;
        }

        // Turn left (counter-clockwise).
        else {
                if ((counterClockwiseAngle >= 0) && (counterClockwiseAngle < TURN_MAX))
                        steering = STEER_LEFT_MAX;
                else if ((counterClockwiseAngle >= TURN_MAX) && (counterClockwiseAngle < TURN_HIG))
                        steering = STEER_LEFT_3;
                else if ((counterClockwiseAngle >= TURN_HIG) && (counterClockwiseAngle < TURN_LOW))
                        steering = STEER_LEFT_2;
                else if ((counterClockwiseAngle >= TURN_LOW) && (counterClockwiseAngle < TURN_MIN))
                        steering = STEER_LEFT_1;
                else
                        steering = STEER_NEUTRAL;
        }
}


/* -----------------------------------------------------------------------------
 * Function: read_kill_switch()
 * Purpose: Safety switch to turn off the car by user
 * Description: PIC listens to the radio signal line, if there is a change from
 * neutral then the function will return 1
 * Return value: 0 for false, 1 for true
 * -------------------------------------------------------------------------- */
int read_kill_switch()
{
   int tty;
   struct termios kill_term, old_term;
   char kill[1];
   int kill_digit;
   tty = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
        
   tcgetattr(tty, &old_term);
   kill_term.c_cflag = B9600;
   kill_term.c_cflag |= CS8;
   kill_term.c_cflag |= CREAD;
   kill_term.c_iflag = IGNPAR | IGNBRK;
   kill_term.c_cflag |= CLOCAL;
   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&kill_term);

   read(tty,kill,1);
   tcsetattr(tty,TCSANOW,&kill_term);
   close(tty);
   kill_digit = strtod(kill,NULL);
   return kill_digit;
}

/* -----------------------------------------------------------------------------
 * Function: read_bumper_sensor()
 * Purpose: signals the car that the orange cone has been touched
 * Description: PIC sets kill variable to 1 and returns it when called
 * Return value: 0 for false, 1 for true
 * -------------------------------------------------------------------------- */
char read_bumper_sensor()
{
   int tty;
   struct termios bump_term, old_term;
   char *bump;
   char y = 'y';
   int bump_digit;
   tty = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
        
   tcgetattr(tty, &old_term);
   bump_term.c_cflag = B9600;
   bump_term.c_cflag |= CS8;
   bump_term.c_cflag |= CREAD;
   bump_term.c_iflag = IGNPAR | IGNBRK;
   bump_term.c_cflag |= CLOCAL;
   tcflush(tty, TCOFLUSH);
   tcsetattr(tty,TCSANOW,&bump_term);
   write(tty,&y,1);
   usleep(100);
   read(tty,bump,1);
   tcsetattr(tty,TCSANOW,&bump_term);
   close(tty);
   //returns y if closed, z if open
   return *bump;
}
