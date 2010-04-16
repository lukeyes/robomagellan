//init all peripherals
//verify all peripherals
//output status to screen
//receive gps coordinates
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "device_threads.h"
#include "sonar.h"
#include "compass.h"
#include "camera.h"
#include "gps.h"

void init_state() {
    static int first = 1;
    int retval;

    if(first) {
        if(debug)
            spawn_debug_thread();

#if 0
        // initialize sonar sensors
        retval = sonar_init();
        if(retval != SONAR_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
            next_state = ERROR_STATE;
            return;
        }
 #endif
        //initialize compass
        retval = compass_init();
        if(retval != COMPASS_NO_ERROR) {
            snprintf(state_data.error_str, sizeof(state_data.error_str), SONAR_ERROR_STR(retval));
            next_state = ERROR_STATE;
            return;
        }
    
        // Spawn device threads
        spawn_device_threads();
        first = 0;
    }
    next_state = NAVIGATION_STATE;
}
#if 0
int init_gps() {
        int tty = initialize_gps();
        if(tty < 0){
                if(debug)
                        fprintf(stderr,"Problem initializing GPS\n");
                return 0;
        }

        else {
                if(debug)
                        fprintf(stderr,"GPS initialization successful");
                return tty;
        }
}

int init_i2c() {
        int tty = initialize_i2c();
        if(tty < 0){
                if(debug)
                        fprintf(stderr,"Problem initializing i2c\n");
                return 0;       
        }

        else {
                if(debug)
                        fprintf(stderr,"i2c initialization successful");
                return tty;
        }
}

int init_camera()
{
        int tty = initialize_camera();
        if(tty < 0) {
                if(debug)
                        fprintf(stderr,"Problem initializing camera\n");
                return 0;       
        }

        else {
                if(debug)
                        fprintf(stderr,"camera initialization successful");
                return tty;
        }

}
int init_sonar(int tty)
{
        int retval = initialize_sonar(tty);
        if(debug) {
                if(retval==0)
                        fprintf(stderr,"sonar initialization successful");
        }
}

int init_compass(int tty)
{
        return 0;
}
#endif
