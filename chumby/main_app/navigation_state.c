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
        if(kill_switch_asserted) {
            next_state = PAUSE_STATE;
            return;
        }
        usleep(1000);
#ifdef USE_SONAR
        // Is there an object on the path?
        if((state_data.front_sonar < OBJECT_DETECT_THRESH) && sonar_initialized) {
            next_state = OBJECT_AVOIDANCE_STATE;
            return;
        }
#endif
    
#ifdef USE_COMPASS
        rel_heading = get_relative_heading(state_data.compass_heading, state_data.target_heading);
        car_set_turn(rel_heading * 100 / 180);
#endif
#ifdef USE_CAMERA
        if(state_data.cone_position) {
            next_state = TRACK_STATE;
            return;
        }
#endif
#ifdef USE_GPS
        // Are we at a waypoint?
        if (state_data.target_distance <= 6) {
            // Is this the end point?
            if(current_waypoint_idx == num_waypoints-1) {
                next_state = TRACK_STATE;
                return;
            } else {
                ++current_waypoint_idx;
                state_data.target_lat = waypoints[current_waypoint_idx*2];
                state_data.target_long = waypoints[current_waypoint_idx*2+1];
            }
        }
#endif
    }
}
