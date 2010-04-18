#include <stdlib.h>
#include <stdio.h>
#include "main.h"

void object_avoidance_state() {
#if 0
    car_set_speed(65);
    while(front_sonar < THRESH || right_sonar <  THRESH) {
        if(front_sonar < THRESH) {
            if(front_sonar > 95) //do 20% turn
                turn_left(20);
            else if(front_sonar > 60) //do 60% turn
                turn_left(60);
            else if(front_sonar > 25)
                turn_left(100); //do max turn
            else
                stop_car();
        }
        else
            turn_center();
    }

#endif
    next_state = TRACK_STATE;
}
