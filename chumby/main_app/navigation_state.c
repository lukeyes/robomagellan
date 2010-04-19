#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "car.h"
#include "navigation_state.h"

void navigation_state() {
    car_set_speed(navigation_speed);
    car_set_turn(0);
    while(1) {
        usleep(1000);
        // Is there an object on the path?
        if(state_data.front_sonar < OBJECT_DETECT_THRESH) {
            next_state = OBJECT_AVOIDANCE_STATE;
            return;
        }
    }
}
