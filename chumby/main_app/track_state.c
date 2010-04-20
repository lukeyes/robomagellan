#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "track_state.h"
#include "car.h"

void cone_feedback(int pixel_location);

void track_state() {
    if(cone_find() == 1);
    	cone_track();
      
    next_state = DONE_STATE;
}

//Searches for cone by making a big slow loop
//A fail to find algorithm should be added
int cone_find(){
	int turning = 0;
	car_set_speed(SEARCH_SPEED);//Need to change
	while(state_data.cone_position == 0)
	{
		usleep(1000);
        	// Is there an object on the path?
        	if(state_data.front_sonar < OBJECT_DETECT_THRESH) {
        	    	next_state = OBJECT_AVOIDANCE_STATE;
            		return;
        	}
        	if(turning == 0){
        		turning = 1;
        		car_set_turn(20);
        	}
        	usleep(150000);	
        }
        car_set_turn(0);
        return 1;  
}


//Once cone is found navigate within a feedback loop to locate cone. Attempt to keep cone centered.
void cone_track(){
	int found = 0;
	int last_pixel = 0;
	int tries = 0;
	car_set_speed(TRACK_SPEED);
	while(found == 0)
	{
		if(state_data.cone_position == 0){
			cone_feedback(last_pixel);
			usleep(150000);
			tries++;
		}
		else{
			last_pixel = state_data.cone_position;
			cone_feedback(last_pixel);
			usleep(150000);

			if(state_data.front_sonar < DETECT_THRESHOLD)
			{
				car_set_speed(0);
				found = 1;
			}
		}
		if(tries > 10)
		{
			tries = 0;
			cone_find();
		}
	}
}

void cone_feedback(int pixel_location)
{
	int turningl = 0;
	int turningr = 0;
	if(pixel_location <= LEFT_PIXEL_THRESHOLD)
	{
		if (turningl == 0)
		{
			car_set_turn(-TURN_RATE);
			turningl = 1;
		}
	} 
	else if(pixel_location >= RIGHT_PIXEL_THRESHOLD)
	{
		if (turningr == 0)
		{
			car_set_turn(TURN_RATE);
			turningr = 1;
		}
	}
	else
	{
		if(turningl != 0 || turningr != 0)
		{
			turningl = 0;
			turningr = 0;
			car_set_turn(0);
		} 	
	}	
}
