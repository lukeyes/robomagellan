#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "track_state.h"
#include "car.h"

void cone_feedback(int pixel_location);
int cone_find();
void cone_track();

void track_state() {
    int last_cone_pos=0;
    int turn=0;
#if 0
    if(cone_find() == 1);
    cone_track();
#endif
    while (1) {
        usleep(1000);

        car_set_speed(4);

        if((last_cone_pos = state_data.cone_position)) {
            if (state_data.front_sonar < .8) {
                next_state = DONE_STATE;
                return;
            } else {
                turn = last_cone_pos - 79;
                turn /= 1.2;
                car_set_turn(turn);
            }
        } else {
                //car_set_turn();
        }
    }
      
}

//Searches for cone by making a big slow loop
//A fail to find algorithm should be added
int cone_find(){
    int turning = 0;
    car_set_speed(SEARCH_SPEED);//Need to change
    car_set_turn(0);
    while(state_data.cone_position == 0)
    {
        usleep(1000);

        // Is there an object on the path?
        if(state_data.front_sonar < OBJECT_DETECT_THRESH) {
            next_state = OBJECT_AVOIDANCE_STATE;
            return 0;
        }
        if(turning == 0){
            turning = 1;
            car_set_turn(20);
        }
        usleep(150000); 

    }
    //Center wheels and come to a complete stop so that we have a reasonable expectation of being able to see the cone for the tracking function
    car_set_turn(0);
    car_set_speed(0);
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
        if(tries >= 50)
        {
            tries = 0;
            cone_find();
        }
    }
}

//If tracked pixel is on the left side of the camera, turn left, if it is to the right, turn right, otherwise, stay in the middle
//Pixel range is 1 to ~159
void cone_feedback(int pixel_location)
{
    int turningl = 0;
    int turningr = 0;
    if(pixel_location <= LEFT_PIXEL_THRESHOLD)
    {
        if (turningl == 0)
        {
            //Turn Left
            car_set_turn(-TURN_RATE);
            turningl = 1;
        }
    } 
    else if(pixel_location >= RIGHT_PIXEL_THRESHOLD)
    {
        if (turningr == 0)
        {
            //Turn Right
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
