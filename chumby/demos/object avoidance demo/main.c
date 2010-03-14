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
	int front_sonar;
	int right_sonar;

	
	/* check for debug flag */
	if (argc > 1 && !strncmp("-d", argv[1],2))
		debug = 1;

	tty =  initialize_i2c();
	initialize_sonar(tty);
	init_car();

	drive_forward(100);

	while(1) {
		take_range( tty );
		front_sonar = get_front_range(tty);
		if(debug)
			fprintf(stderr,"Before avoid, front sonar: %d",front_sonar);
		if (front_sonar < THRESH) { //4.8 meters, this should be safe
			if(debug)
				fprintf(stderr,"Detected object at sonar value: %d",front_sonar);
			avoid_object();
		if(debug)
			fprintf(stderr,"After avoid, front sonar: %d\nright sonar: %d",front_sonar,get_right_range(tty));
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
	fprintf(stderr,"first\n");
	take_range(tty);
	fprintf(stderr,"second\n");
	front_sonar = get_front_range(tty);
	fprintf(stderr,"third\n");
	right_sonar = get_right_range(tty);
	fprintf(stderr,"fourth\n");
	drive_forward(20);
	fprintf(stderr,"five\n");
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






