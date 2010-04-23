#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "car.h"
#include "object_avoidance_state.h"

void object_avoidance_state() {
    double last_target_heading = state_data.target_heading;
    car_set_speed(object_avoidance_speed);

    while(state_data.front_sonar < OBJECT_DETECT_THRESH ||
          state_data.right_sonar <  OBJECT_DETECT_THRESH) {

        if(kill_switch_asserted) {
            next_state = PAUSE_STATE;
            return;
        }

        if(state_data.front_sonar < OBJECT_DETECT_THRESH) {
            if(state_data.front_sonar > 2) //do 20% turn
                car_set_turn(-20);
            else if(state_data.front_sonar > 1) //do 60% turn
                car_set_turn(-60);
            else if(state_data.front_sonar > 0.5)
                car_set_turn(-100); //do max turn
            else {
                car_set_speed(0);
                return;
            }
        }
        else
            car_set_turn(0);
       usleep(1000);
    }

    next_state = NAVIGATION_STATE;
}
