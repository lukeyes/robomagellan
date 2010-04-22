#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "car.h"
#include "gps.h"
#include "navigation_state.h"

void navigation_state() {
    double rel_heading;
    if(car_initialized) {
        car_set_speed(navigation_speed);
        car_set_turn(0);
    }
    while(1) {
        usleep(1000);
#if 1
        // Is there an object on the path?
        if((state_data.front_sonar < OBJECT_DETECT_THRESH) && sonar_initialized) {
            next_state = OBJECT_AVOIDANCE_STATE;
            return;
        }
#endif
    
#if 1
        rel_heading = get_relative_heading(state_data.compass_heading, state_data.target_heading);
        car_set_turn(rel_heading * 100 / 180);
#endif
    }
}
