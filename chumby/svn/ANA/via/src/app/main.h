/* -----------------------------------------------------------------------------
 * File: main.h
 * Description: Defines constants and function prototypes.
 * -------------------------------------------------------------------------- */

#ifndef _MAIN_H_
#define _MAIN_H_

// Constants. //////////////////////////////////////////////////////////////////
// Sonar steps. See notes below for int sonarStep.
#define SN_STEP_NULL_STEP                       -1
#define SN_STEP_NO_OBS                          0
#define SN_STEP_ENCOUNTERED_OBS         1
#define SN_STEP_FOLLOW_OBS                      2
#define SN_STEP_LOST_OBS_ON_RIGHT       3
#define SN_STEP_OBS_WHILE_FOLLOW        4
#define SN_STEP_CROSSED_GOAL_LINE       5
#define SN_STEP_CLEAR                           6

// Steering constants.
#define STEER_LEFT_MAX  110
#define STEER_LEFT_3    112
#define STEER_LEFT_2    113
#define STEER_LEFT_1    114
#define STEER_NEUTRAL   115
#define STEER_RGHT_1    117
#define STEER_RGHT_2    118
#define STEER_RGHT_3    119
#define STEER_RGHT_MAX  120
#define SN_LEFT_TURN    -90;
#define SN_RIGHT_TURN   90;
#define SN_SMALL_LEFT   -15;
#define SN_SMALL_RIGHT  15;

// Sonar distances.
#define SN_TRIP_DISTANCE        60 // Distance in inches that we should stay away
                                                           // from obstacles.
#define SN_ALMOST_ZERO          1  // Sonar doesn't read more than 3cm, so anything
                                                           // less than 1 inch is about 0.
#define SN_FOLLOW_DISTANCE      40 // How close to the obstacle we should stay on
                                                           // the right.
#define SN_GL_EPSILON           12 // Distance we should say is close enough to the
                                                           // goal line. This is absolute error in the
                                                           // North-South direction.
#define INCHES_PER_DEG_LONG     3157894.73684 // In San Francisco, hopefully.
#define INCHES_PER_DEG_LAT      5187969.92827

// Sonar direction tolerances, in degrees (eg |difference in degrees| <= small
// || |difference in degrees| >= large).
#define DIR_EPSILON_SMALL       5
#define DIR_EPSILON_LARGE       355

#define TOKEN_SEPARATOR "," // Used for GPS parsing.

// Used for steering in GPS and sonar.
#define DEAD_ZONE 20; // How big the total dead zone should be.
#define TURN_MAX (1*(180-(20/2))/4)
#define TURN_HIG (2*(180-(20/2))/4)
#define TURN_LOW (3*(180-(20/2))/4)
#define TURN_MIN (4*(180-(20/2))/4)

// Used for GPS.
#define QUAD0 0
#define QUAD1 1
#define QUAD2 2
#define QUAD3 3

// Boolean values.
#define TRUE    1
#define FALSE   0

// SN = Sonar
// Number of data points we expect to get from the sonar sensors.
#define SN_NUM_DATA_POINTS      10

// Distance in inches that we should stay away from obstacles.
#define SN_TRIP_DISTANCE        60

// Sonar steps. See notes below for int sonarStep.
#define SN_STEP_NO_OBS                          0
#define SN_STEP_ENCOUNTERED_OBS         1
#define SN_STEP_FOLLOW_OBS                      2
#define SN_STEP_LOST_OBS_ON_RIGHT       3
#define SN_STEP_OBS_WHILE_FOLLOW        4
#define SN_STEP_CROSSED_GOAL_LINE       5

// Function prototypes. ////////////////////////////////////////////////////////
// Camera stuff. ------------------------------------------------------------ //
int open_camera(char *angle);
char calculate_next_direction(int angle);

// Driving stuff. ----------------------------------------------------------- //
void send_pwm_steer(char value);
void send_pwm_motor(char value);
char SteerTo (double myDirection, double desiredDirection);
int read_kill_switch();
int read_bumper_sensor();

//Compass stuff. ------------------------------------------------------------ //
double get_heading();


// GPS stuff. --------------------------------------------------------------- //
int open_gps();
void ReadData(double data[], int TTY);
double Delta(double a, double b);
double OppositeAngle(double angle);
int CalcQuadrant(double coneLat, double coneLong, double carLat,
        double carLong);
double CalcNRef(int quadrant, double coneLat, double coneLong, double carLat,
                double carLong);
char Turn(int quadrant, double nRef, double heading);
double DirectionTo (double carLat, double carLong, double coneLat,
        double coneLong);
double DistanceBetween (double coord1Lat, double coord1Long, double coord2Lat,
        double coord2Long);

// Sonar stuff. ------------------------------------------------------------- //
double AvoidObstacle (double frontDist, double rightDist, double carDirection,
        int step, double oldIntendedDirection, double carLat, double carLong,
        double coneLat, double coneLong);
int GetSonarStep (int currentStep, double carLat, double carLong,
        double encObsLat, double encObsLong, double coneLat, double coneLong,
        double carDirection, double intendedDirection, double frontDist,
        double rightDist);
int OnGoalLine (double carLat, double carLong, double encObsLat,
        double encObsLong, double coneLat, double coneLong);
void ReadSonarData (float sonarData[], int tty);
int open_sonar();
double get_front_sonar();
double get_right_sonar();
#endif
