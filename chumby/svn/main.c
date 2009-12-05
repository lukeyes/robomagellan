#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "main.h"

int main(int argc, char *argv[]) {
//      master_motor(speed0);
                sleep(5);
        // Variables ------------------------------------------------------------ //
        // GPS Variables -------------------------------------------------------- //
        int gps_tty;
        double gps_data[2];
        double coneLat = strtod(argv[1], NULL);
        double coneLong = strtod(argv[2], NULL);
        double carLat;
        double carLong;
        long int gps_turn;
        gps_tty = open_gps();

        // Camera Variables ----------------------------------------------------- //
        int camera_tty;
        int i;
        int servo_angle;
        char string[BUFSIZ];
        char *str;
        int camera_turn;

        /* Compass variables. --------------------------------------------------- */
        double carDirection = get_heading();

        // Sonar variables. ----------------------------------------------------- //
        // Interface with sonar hardware -- forward and right looking sonars.
///     int sonar_front_tty;
///     double sonar_front_data[SN_NUM_DATA_POINTS];
///     sonar_front_tty = open_sonar();
///     int sonar_right_tty;
///     double sonar_right_data[SN_NUM_DATA_POINTS];
///     sonar_right_tty = open_sonar();

        // Boolean flag -- are we in the middle of tracing our way around an
        // obstacle, or not? This will be set when something gets in our way during
        // normal operation, and won't be unset until we cross the goal line at a
        // point closer to the cone than we started. The goal line is the straight
        // line between us and the cone when we first encounter the obstacle.
        // NOTE: It may turn out that this is just a rename of int sonar below.
        int avoidingObstacle = FALSE;

        // Sonar values - distance in feet in inches and in back.
        double snFrontDist;
        double snRightDist;

        // Our GPS coordinates that we save when we encounter an obstacle. Used to
        // determine where the goal line is.
        double encObsLat;  // Our latitude when we encountered the obstacle.
        double encObsLong; // Our longitude when we encountered the obstacle.

        // Where we want to be facing.
        static double intendedDirection; // NOTE: Set this to GPS direction by
                                                                         // default.
        double currentDirection = 0; // NOTE: This needs to be set to compass output.

        // What step of the avoidance routine we're on.
        // Step 0: No obstacles in the way, let gps do the driving.
        // Step 1: Obstacle encountered, turn left.
        // Step 2: Follow obstacle, keeping it to our right side. Branches to either
        //      Step 3, Step 4, or Step 5.
        // Step 3: We were following an obstacle, but don't see it on our right side
        //      anymore. So, we turn right and go to Step 3. A hardcoded right turn will
        //      probably be necessary.
        // Step 4: We were following an obstacle, but see another obstacle in front
        //      of us. Turn left and go to Step 2. A hardcoded left turn will probably
        //      be necessary.
        // Step 5: We were following an obstacle, but have just crossed the goal
        //      line. Orient ourselves back towards cone and let gps do the driving
        //  again (go to Step 0).
        int sonarStep = SN_STEP_NO_OBS;

        // Data variables to process later. ------------------------------------- //
        int kill, bump, sonar, color;

        kill = 0;

        while (1) {
                //    kill = read_kill_switch()

                /****************************GPS processing***************************/
                
                 ReadData(gps_data,gps_tty);
                 carLat = gps_data[0];
                 carLong = gps_data[1];
                 printf("car latitude is: %f vs. cone latitude: %f\n",carLat,coneLat);
                 printf("car longitude is: %f vs. cone longitude: %f\n",carLong,coneLong);
                 sleep(1);

                 gps_turn = get_turn_value(coneLat,coneLong,gps_tty,carLat,carLong); //get the turn value
                 printf("\n Turn value is: %ld\n",gps_turn);
                 

                /**************************Camera processing**************************/
                camera_tty = open_camera(string);
                i = 0;
                str = strtok(string, " ");
                //      printf("first token: %s\n",str);
                str = strtok(NULL, " ");
                //      printf("second token: %s\n",str);
                //      str = strtok(NULL," ");
                //      printf("third token: %s\n",str);
                servo_angle = atoi(str);
                for (i = 0; i < strlen(string); i++)
                        string[i] = '\0';
                *string = 0;
                *str = 0;
                /**///      fprintf(stderr,"servo angle is: %ld\n",servo_angle);
                camera_turn = calculate_next_direction(servo_angle);

                //      send_pwm_steer(turn);
                fprintf(stderr, "int sent to servo: %c\n", camera_turn);

                /*************************Sonar Processing****************************/
                // Get sonar data.
///             ReadSonarData((float*) sonar_front_data, sonar_front_tty);
///             ReadSonarData((float*) sonar_right_data, sonar_right_tty);
                
                // If the front sonar sensor reads an object directly ahead, then we need
                // to enter obstacle avoidance mode. Otherwise, do nothing and let the GPS
                // do its work (straight-line path to cone).
                // Get the average value of the data points.
                snFrontDist = get_front_sonar();
                snRightDist = get_right_sonar();
                
                // Determine if there's something too close in front.
                if (snFrontDist < SN_TRIP_DISTANCE) {
                        // sonar, avoidingObstacle, and (sonarStep == 0) may be redundant
                        // variables. Just keeping them all for now.
                        sonar = TRUE;
                        sonarStep = SN_STEP_ENCOUNTERED_OBS;
                        avoidingObstacle = TRUE;

                        // Save initial gps coordinates before going around obstacle.
                        encObsLat = carLat;
                        encObsLong = carLong;
                }
                else
                        sonar = FALSE;

                /***************************Begin Logic******************************/
                int master_steer = gps_turn;
                int master_speed = 107;

                bump = read_bumper_sensor();

                if (kill == 1)
                        break;

                /*if coordinate is too far away, if sonar detects object, else use GPS*/
                // NOTE: .005 is approximately 9 feet.
                // If we're not close to the cone.
                if (abs(carLat - coneLat) > .005 || abs(carLong - coneLong) > .005) {
                        if(avoidingObstacle == TRUE /*|| sonar == TRUE*/) {
                                sonarStep = GetSonarStep (sonarStep, carLat, carLong, coneLat,
                                        encObsLat, encObsLong, coneLong, carDirection,
                                        intendedDirection, snFrontDist, snRightDist);

                                // We break out of the obstacle avoidance mode if we're clear of
                                // the obstacle.
                                if (sonarStep == SN_STEP_CLEAR) {
                                        avoidingObstacle == FALSE;
                                }
                                else {
                                        intendedDirection = AvoidObstacle (snFrontDist, snRightDist,
                                                carDirection, sonarStep, intendedDirection, carLat,
                                                carLong, coneLat, coneLong);
                                        master_steer = SteerTo (currentDirection, intendedDirection);
                                        // NOTE: Above doesn't exist.
                                        master_speed = 105; // NOTE: this should be some slow value.
                                }
                        }
                        /*Use GPS data to turn*/
                        else {
                                master_steer = gps_turn;
                                master_speed = 106;
                        }
                }

                // If we're close to the target
                //if sonar detects object, else use camera information to update motor
                else
                {
                                master_steer = camera_turn;
                                master_speed = 106;
                            if (bump == 1)
                                    break;
                }

                send_pwm_steer(master_steer);
                send_pwm_motor(master_speed);
        }
                send_pwm_steer(115);
        return 0;
}
