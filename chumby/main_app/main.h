/*  Author: Michael Oritz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 4/1/2010
 *
 *  main.h
 *  This file contains sensory data and state machine info that is global
 *  all robomagellan control modules.
 */

struct {
    double compass_heading; // compass heading in degrees
    double target_bearing;  // bearing to target in degress
    double target_distance;  // distance to target in meters
    int front_sonar;    // distance from front sonar to object in cm
    int right_sonar;    // distance from right sonar to object in cm
    int speed;  // -100 reverse, 0 stop, +100 forward
    int turn;   // -100 left, 0 center, +100 forward
} sensory_data;

enum {
    INIT_STATE = 0,
    NAVIGATION_STATE,
    OBJECT_AVOIDANCE_STATE,
    TRACK_CONE_STATE,
    DONE_STATE
} STATES;

int next_state;

int gps_tty;
int compass_tty;
int sonar_tty;
int camera_tty;
