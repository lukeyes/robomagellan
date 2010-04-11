/*  Author: Michael Oritz
 *  Email: mtortiz.mail@gmail.com
 *  Date: 4/1/2010
 *
 *  main.c
 *  This is the main state machine control module for the robomagellan
 *  project.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "sonar.h"
#include "compass.h"
#include "camera.h"
#include "gps.h"
#include "done_state.h"
#include "track_state.h"
#include "object_avoidance_state.h"
#include "navigation_state.h"
#include "init_state.h"
#include "main.h"

void * gps_task(void * ptr);
void * compass_task(void * ptr);
void * front_sonar_task(void * ptr);
void * right_sonar_task(void * ptr);
void * camera_task(void * ptr);

void exit_routine (int sig);

int main () {
    int current_state;
    pthread_t gps_t, compass_t, front_sonar_t, right_sonar_t, camera_t;

    // Control-C detection
    signal(SIGINT, exit_routine);

    // Launch all threads
    pthread_create(&gps_t, NULL, gps_task, NULL);
    pthread_create(&compass_t, NULL, compass_task, NULL);
    pthread_create(&front_sonar_t, NULL, front_sonar_task, NULL);
    pthread_create(&right_sonar_t, NULL, right_sonar_task, NULL);
    pthread_create(&camera_t, NULL, camera_task, NULL);

    current_state = next_state = INIT_STATE;
    while (1) {
        current_state = next_state;
        switch (current_state) {
            case INIT_STATE:
                init_state();
                break;
            case NAVIGATION_STATE:
                navigation_state();
                break;
            case OBJECT_AVOIDANCE_STATE:
                object_avoidance_state();
                break;
            case TRACK_CONE_STATE:
                track_state();
                break;
            case DONE_STATE:
                done_state();
                break;
        }
        fprintf(stderr, "main\n\n");
        sleep(1);
    }
}

void * gps_task(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        fprintf(stderr, "gps_task\n");
        sleep(1);
    }
    return ret_ptr;
}

void * compass_task(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        fprintf(stderr, "compass_task\n");
        sleep(1);
    }
    return ret_ptr;
}

void * front_sonar_task(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        fprintf(stderr, "front_sonar_task\n");
        sleep(1);
    }
    return ret_ptr;
}

void * right_sonar_task(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        fprintf(stderr, "right_sonar_task\n");
        sleep(1);
    }
    return ret_ptr;
}

void * camera_task(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        fprintf(stderr, "camera_task\n");
        sleep(1);
    }
    return ret_ptr;
}

void exit_routine (int sig) {
    fprintf(stderr, "exiting program\n");
    exit(0);
}
