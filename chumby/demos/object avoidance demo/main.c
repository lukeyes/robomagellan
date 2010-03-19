#include <stdio.h>
#include <string.h>
#include "usb_i2c.h"
#include "sonar.h"
#include "drive_control.h"

#define THRESH 190  //about 4.8 meters
int tty;
int debug = 0;

void avoid_object();

int main(int argc, char * argv[])
{
	
	/*Initialize all variables*/
	int front_sonar;
	int right_sonar;
    char key = 0;

	/* check for debug flag */
	if (argc > 1 && !strncmp("-d", argv[1],2))
		debug = 1;

	tty =  initialize_i2c();
	initialize_sonar(tty);
	init_car();

    do {
	    fprintf(stderr,"enter 'g' to start the car: ",front_sonar);
        key = (char)getc(stdin);
    } while (key != 'g');

	drive_forward(50);

	while(1) {
		take_range( tty );
		front_sonar = get_front_range(tty);
		if(debug)
			fprintf(stderr,"Before avoid, front sonar: %d",front_sonar);
		if (front_sonar < THRESH) {
			if(debug)
				fprintf(stderr,"Detected object at sonar value: %d",front_sonar);
			avoid_object();
		if(debug)
			fprintf(stderr,"After avoid, front sonar: %d\nright sonar: %d",front_sonar,get_right_range(tty));
			stop_car();
		}
	}
	close(tty);
	tty = 0;
    return 0;
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
			if(front_sonar > 95) //do 20% turn
				turn_left(20);
			else if(front_sonar > 60) //do 60% turn
				turn_left(60);
			else if(fron_sonar > 15)
				turn_left(100); //do max turn
            else
                stop_car();
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
