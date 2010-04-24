/*  Author: Michael Oritz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 4/1/2010
 *
 *  main.h
 *  This file contains sensory data and state machine info that is global
 *  all robomagellan control modules.
 */

//#define USE_KILL_SWITCH
//#define USE_SONAR
//#define USE_COMPASS
#define USE_GPS
//#define USE_CAMERA
//#define USE_CAR

#define OBJECT_DETECT_THRESH 2 //meters

struct {
    char current_state_str [32]; // Current state string
    char error_str [32]; // Error message
    double front_sonar; // distance from front sonar to object in meters
    double right_sonar; // distance from left sonar to object in meters
    double current_lat; // Current latitude in degrees
    double current_long; // Current longitude in degrees
    double target_lat; // Latitude of target in degrees
    double target_long; // Longitude of target in degrees
    double compass_heading; // compass heading in degrees
    double target_heading; // target heading degress
    double target_distance; // distance to target in meters
    int cone_position; // x-position of the cone from camera perspective
    int speed; // -100 reverse, 0 stop, +100 forward
    int turn;  // -100 left, 0 center, +100 forward
} state_data;

enum {
    INIT_STATE = 0,
    NAVIGATION_STATE,
    OBJECT_AVOIDANCE_STATE,
    TRACK_STATE,
    DONE_STATE,
    ERROR_STATE,
    PAUSE_STATE,
    NUM_STATES
} STATES;

#define STATE_STR(__st)  state_strings[__st]
// This is defined in main.c
extern const char * state_strings[NUM_STATES];

int kill_switch_asserted;
int next_state;
int prev_state;
int debug;
int kill_switch_initialized;
int sonar_initialized;
int gps_initialized;
int compass_initialized;
int car_initialized;
int camera_initialized;
