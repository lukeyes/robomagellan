#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "main.h"
#include "sonar.h"
#include "compass.h"
#include "camera.h"
#include "gps.h"
#include "console.h"


void * gps_thread(void * ptr);
void * compass_thread(void * ptr);
void * sonar_thread(void * ptr);
void * camera_thread(void * ptr);
void * debug_output_thread(void * ptr);

static char * debug_output = 
        "  current state: \n"
        "          error: \n"
        "    front sonar: \n"
        "     left sonar: \n"
        "    current lat: \n"
        "   current long: \n"
        "     target lat: \n"
        "    target long: \n"
        "compass heading: \n"
        " target heading: \n"
        "target distance: \n"
        "   camera servo: \n"
        "          speed: \n"
        "           turn: \n";

void spawn_device_threads() {
    pthread_t gps_t, compass_t, sonar_t, camera_t;

    // Launch all threads
    pthread_create(&gps_t, NULL, gps_thread, NULL);
    pthread_create(&compass_t, NULL, compass_thread, NULL);
    pthread_create(&sonar_t, NULL, sonar_thread, NULL);
    pthread_create(&camera_t, NULL, camera_thread, NULL);
}

void spawn_debug_thread () {
    pthread_t debug_output_t;
    pthread_create(&debug_output_t, NULL, debug_output_thread, NULL);
}

void * gps_thread(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        sleep(1);
    }
    return ret_ptr;
}

void * compass_thread(void * ptr) {
    void * ret_ptr = NULL;
    double retval;
    while (1) {
#ifdef USE_COMPASS
        retval = compass_get_heading();
        if(retval < 0)
            snprintf(state_data.error_str, sizeof(state_data.error_str), COMPASS_ERROR_STR((int)retval));
        else
            state_data.compass_heading = compass_get_heading();
#endif
        sleep(1);
    }
    return ret_ptr;
}

void * sonar_thread(void * ptr) {
    void * ret_ptr = NULL;
    int retval;
    while (1) {
#ifdef USE_SONAR
        retval = sonar_take_range();
        if(retval != SONAR_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
        } else {
            // Read front sonar range
            retval = sonar_get_front();
            if(retval < 0)
                snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
            else
                state_data.front_sonar = retval;

            // Read left sonar range
            retval = sonar_get_left();
            if(retval < 0)
                snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
            else
                state_data.left_sonar = retval;
        }
#else
        sleep(1);
#endif
    }
    return ret_ptr;
}

void * camera_thread(void * ptr) {
    void * ret_ptr = NULL;
    while (1) {
        sleep(1);
    }
    return ret_ptr;
}

void * debug_output_thread(void * ptr) {
    void * ret_ptr = NULL;
    // Initialize debug display 
    console_clrscr();
    console_gotoxy(0, 0);
    fprintf(stderr,debug_output);
    while (1) {
        console_gotoxy(18, 1);
        fprintf(stderr,"%-32s", state_data.current_state_str);
        console_gotoxy(18, 2);
        fprintf(stderr,"%-32s", state_data.error_str);
        console_gotoxy(18, 3);
        fprintf(stderr,"%6.2fm", state_data.front_sonar);
        console_gotoxy(18, 4);
        fprintf(stderr,"%6.2fm", state_data.left_sonar);
        console_gotoxy(18, 5);
        fprintf(stderr,"%6.2fº", state_data.current_lat);
        console_gotoxy(18, 6);
        fprintf(stderr,"%6.2fº", state_data.current_long);
        console_gotoxy(18, 7);
        fprintf(stderr,"%6.2fº", state_data.target_lat);
        console_gotoxy(18, 8);
        fprintf(stderr,"%6.2fº", state_data.target_long);
        console_gotoxy(18, 9);
        fprintf(stderr,"%6.2fº", state_data.compass_heading);
        console_gotoxy(18, 10);
        fprintf(stderr,"%6.2fº", state_data.target_heading);
        console_gotoxy(18, 11);
        fprintf(stderr,"%6.2fm", state_data.target_distance);
        console_gotoxy(18, 12);
        fprintf(stderr,"%6.2fº", state_data.camera_servo);
        console_gotoxy(18, 13);
        fprintf(stderr,"%6d%%", state_data.speed);
        console_gotoxy(18, 14);
        fprintf(stderr,"%6d%%", state_data.turn);
        console_gotoxy(0, 17);
        usleep(500000);
    }
    return ret_ptr;
}
