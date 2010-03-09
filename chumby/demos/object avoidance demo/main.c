#include <stdio.h>
#include <string.h>
#include "usb_i2c.h"
#include "sonar.h"
#include "drive_control.h"

#define THRESH 190
int tty;
int debug = 0;

void avoid_object();

int main(int argc, char * argv[])
{
	
	/*Initialize all variables*/
	int tty;
	int front_sonar;
	int right_sonar;

	/* check for debug flag */
	if (!strncmp("-d", argv[1],2))
		debug = 1;

	tty =  initialize_i2c();
	initialize_sonar(tty);
	init_car();

	drive_forward(30);

	while(1) {
		take_range( tty );
		front_sonar = get_front_range(tty);
		if (front_sonar < THRESH) { //4.8 meters, this should be safe
			avoid_object();
			stop();
		}
	}
	close(tty);
	tty = 0;
}

void avoid_object()
{
	int front_sonar;
	int right_sonar;
	take_range(tty);
	front_sonar = get_front_range(tty);
	right_sonar = get_right_range(tty);
	drive_forward(20);
	while(front_sonar < THRESH || right_sonar <  THRESH) {
		if(front_sonar < THRESH) {
			if(front_sonar > 118) //do 20% turn
				turn_left(20);
			else if(front_sonar > 60) //do 60% turn
				turn_left(60);
			else
				turn_left(100); //do max turn
		}
		else
			turn_center();

		take_range(tty);
		front_sonar = get_front_range(tty);
		right_sonar = get_right_range(tty);

		if(debug) {
			fprintf(stderr,"front sonar: %d",front_sonar);
			fprintf(stderr,"right sonar: %d",right_sonar);
		}
	}
}






