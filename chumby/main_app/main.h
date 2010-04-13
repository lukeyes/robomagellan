/*  Author: Michael Oritz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 4/1/2010
 *
 *  main.h
 *  This file contains sensory data and state machine info that is global
 *  all robomagellan control modules.
 */

struct {
    char current_state_str [16]; // Current state string
    double front_sonar; // distance from front sonar to object in meters
    double left_sonar; // distance from left sonar to object in meters
    double current_lat; // Current latitude in degrees
    double current_long; // Current longitude in degrees
    double target_lat; // Latitude of target in degrees
    double target_long; // Longitude of target in degrees
    double compass_heading; // compass heading in degrees
    double target_heading; // target heading degress
    double target_distance; // distance to target in meters
    double camera_servo; // camera servo angle in degrees 
    int speed; // -100 reverse, 0 stop, +100 forward
    int turn;  // -100 left, 0 center, +100 forward
} state_data;

enum {
    INIT_STATE = 0,
    NAVIGATION_STATE,
    OBJECT_AVOIDANCE_STATE,
    TRACK_STATE,
    DONE_STATE
} STATES;

int next_state;
int debug;
